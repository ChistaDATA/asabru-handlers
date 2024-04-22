#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "Utils.h"
#include "http/http_message.h"
#include <fstream>
#include <mutex>
#include <nlohmann/json.hpp>

class HTTPBasicLogoutCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};