#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "http_message.h"
#include <fstream>

class SendFileCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};
