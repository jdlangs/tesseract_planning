#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <gtest/gtest.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_process_managers/taskflow_generators/graph_taskflow.h>
#include <tesseract_common/utils.h>

using namespace tesseract_planning;

class TestGenerator : public TaskGenerator
{
public:
  TestGenerator(std::string name, int conditional_ret_val)
    : TaskGenerator(name), conditional_ret_val_(conditional_ret_val)
  {
  }

  virtual void process(TaskInput, std::size_t) const override { std::cout << "Task " << name_ << std::endl; }

  virtual int conditionalProcess(TaskInput, std::size_t) const override
  {
    std::cout << "Task " << name_ << std::endl;
    return conditional_ret_val_;
  }

private:
  int conditional_ret_val_;
};

class TestObserver : public tf::ObserverInterface
{
public:
  virtual void set_up(size_t) {}

  virtual void on_entry(size_t, tf::TaskView) {}

  virtual void on_exit(size_t, tf::TaskView task_view) { executed_tasks.push_back(task_view.name()); }

  std::vector<std::string> executed_tasks;
};

TEST(GraphTaskflowGenerator, CreateAndRun)
{
  GraphTaskflow graph;
  int A = graph.addNode(std::make_unique<TestGenerator>("A", 0), true);
  int A0 = graph.addNode(std::make_unique<TestGenerator>("A0", 3), true);
  int A1 = graph.addNode(std::make_unique<TestGenerator>("A1", -1), false);
  int A2 = graph.addNode(std::make_unique<TestGenerator>("A2", -1), false);
  int A00 = graph.addNode(std::make_unique<TestGenerator>("A00", -1), false);
  int A01 = graph.addNode(std::make_unique<TestGenerator>("A01", -1), false);
  int A02 = graph.addNode(std::make_unique<TestGenerator>("A02", 1), true);

  ASSERT_NO_THROW(graph.addEdges(A, { A0, A1, A2 }));
  ASSERT_NO_THROW(graph.addEdges(A0, { GraphTaskflow::ERROR_NODE, A00, A01, A02, GraphTaskflow::DONE_NODE }));

  // Adding more than one edge to non-conditional nodes is not allowed
  ASSERT_THROW(graph.addEdges(A1, { A00, A01, A02 }), std::runtime_error);
  ASSERT_THROW(graph.addEdges(A2, { A00, A01, A02 }), std::runtime_error);

  TaskInput input(nullptr, nullptr, {}, nullptr, false, {});

  TaskflowVoidFn done_fn = []() { std::cout << "Done" << std::endl; };
  TaskflowVoidFn error_fn = []() { std::cout << "Error" << std::endl; };
  TaskflowContainer container;
  ASSERT_NO_THROW(container = graph.generateTaskflow(input, done_fn, error_fn));

  // Save the graph visualization
  std::ofstream out_data;
  out_data.open(tesseract_common::getTempPath() + "graph_taskflow-" + tesseract_common::getTimestampString() + ".dot");
  container.taskflow->dump(out_data);
  out_data.close();

  // Run the taskflow with a custom observer that will record the states that get entered
  tf::Executor executor(1);
  auto obs = executor.make_observer<TestObserver>();
  executor.run(*container.taskflow).wait();

  std::vector<std::string> expected_executed_tasks = { "A", "A0", "A02", "Done Callback" };
  ASSERT_EQ(expected_executed_tasks, obs->executed_tasks);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
