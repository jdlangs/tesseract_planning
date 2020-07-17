﻿/**
 * @file interpolated_process_generator.h
 * @brief Generates an interpolator process
 *
 * @author Matthew Powelson
 * @date July 15. 2020
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2020, Southwest Research Institute
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TESSERACT_PROCESS_MANAGERS_INTERPOLATED_PROCESS_GENERATOR_H
#define TESSERACT_PROCESS_MANAGERS_INTERPOLATED_PROCESS_GENERATOR_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <vector>
#include <console_bridge/console.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_process_managers/process_generator.h>

namespace tesseract_planning
{
class InterpolatedProcessGenerator : public ProcessGenerator
{
public:
  using Ptr = std::shared_ptr<InterpolatedProcessGenerator>;
  using ConstPtr = std::shared_ptr<const InterpolatedProcessGenerator>;

  std::function<void()> generateTask(ProcessInput input) override;

  std::function<int()> generateConditionalTask(ProcessInput input) override;

  int conditionalProcess(ProcessInput input) const;

  void process(ProcessInput input) const;

private:
  std::vector<ProcessInput> task_inputs_;
};

}  // namespace tesseract_planning

#endif