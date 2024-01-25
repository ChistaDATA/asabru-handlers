#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "http_message.h"
#include "Utils.h"
#include <mutex>
#include <fstream>

class HttpFileUploadCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};
