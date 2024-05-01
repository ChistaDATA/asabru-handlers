#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"

class IsAuthorizedCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};