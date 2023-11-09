#pragma once
#include "BaseComputationCommand.h"
#include "ComputationContext.h"

class SendHtmlCommand: BaseComputationCommand {
    bool Execute(ComputationContext* context) override;
};
