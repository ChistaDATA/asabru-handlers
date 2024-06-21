#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "http/http_message.h"
#include "Utils.h"
#include <mutex>
#include <fstream>
#include <nlohmann/json.hpp>

class AsyncAPIRequestCommand: BaseComputationCommand {
  bool Execute(ComputationContext* context) override;
};