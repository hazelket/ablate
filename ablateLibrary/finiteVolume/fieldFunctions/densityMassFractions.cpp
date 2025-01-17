#include "densityMassFractions.hpp"
#include <mathFunctions/functionPointer.hpp>

ablate::finiteVolume::fieldFunctions::DensityMassFractions::DensityMassFractions(std::shared_ptr<ablate::finiteVolume::fieldFunctions::CompressibleFlowState> flowStateIn,
                                                                                 std::shared_ptr<ablate::domain::Region> region)
    : ablate::mathFunctions::FieldFunction(
          "densityYi", std::make_shared<ablate::mathFunctions::FunctionPointer>(ablate::finiteVolume::fieldFunctions::CompressibleFlowState::ComputeDensityYiFromState, flowStateIn.get()), {}, region),
      flowState(flowStateIn) {}

#include "registrar.hpp"
REGISTER(ablate::mathFunctions::FieldFunction, ablate::finiteVolume::fieldFunctions::DensityMassFractions, "initializes the densityYi conserved field variables based upon a CompressibleFlowState",
         ARG(ablate::finiteVolume::fieldFunctions::CompressibleFlowState, "state", "The CompressibleFlowState used to initialize"),
         OPT(ablate::domain::Region, "region", "A subset of the domain to apply the field function"));