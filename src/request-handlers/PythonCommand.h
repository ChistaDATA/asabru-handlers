#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "http_message.h"
#include <fstream>

class PythonCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};
