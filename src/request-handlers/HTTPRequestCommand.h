#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "http_message.h"
#include "Utils.h"
#include <mutex>
#include <fstream>
#include <nlohmann/json.hpp>

class HTTPRequestCommand: BaseComputationCommand {
  bool Execute(ComputationContext* context) override;
};