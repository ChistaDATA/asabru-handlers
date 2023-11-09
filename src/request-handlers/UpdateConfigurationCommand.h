#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"
#include "http_message.h"

class UpdateConfigurationCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};
