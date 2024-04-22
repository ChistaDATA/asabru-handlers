#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "Utils.h"
#include "http/http_message.h"
#include <fstream>
#include <mutex>

class HttpFileUploadCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};
