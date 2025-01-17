#include "boxMesh.hpp"
#include <stdexcept>
#include <utilities/mpiError.hpp>
#include <utilities/petscOptions.hpp>
#include "utilities/petscError.hpp"

ablate::domain::BoxMesh::BoxMesh(std::string name, std::vector<std::shared_ptr<FieldDescriptor>> fieldDescriptors, std::vector<std::shared_ptr<modifiers::Modifier>> modifiers, std::vector<int> faces,
                                 std::vector<double> lower, std::vector<double> upper, std::vector<std::string> boundary, bool simplex)
    : Domain(CreateBoxDM(name, faces, lower, upper, boundary, simplex), name, fieldDescriptors, modifiers) {}

ablate::domain::BoxMesh::~BoxMesh() {
    if (dm) {
        DMDestroy(&dm);
    }
}

DM ablate::domain::BoxMesh::CreateBoxDM(std::string name, std::vector<int> faces, std::vector<double> lower, std::vector<double> upper, std::vector<std::string> boundary, bool simplex) {
    std::size_t dimensions = faces.size();
    if ((dimensions != lower.size()) || (dimensions != upper.size())) {
        throw std::runtime_error("BoxMesh Error: The faces, lower, and upper vectors must all be the same dimension.");
    }

    std::vector<DMBoundaryType> boundaryTypes(dimensions, DM_BOUNDARY_NONE);
    for (std::size_t d = 0; d < PetscMin(dimensions, boundary.size()); d++) {
        PetscBool found;
        PetscEnum index;
        PetscEnumFind(DMBoundaryTypes, boundary[d].c_str(), &index, &found) >> checkError;

        if (found) {
            boundaryTypes[d] = (DMBoundaryType)index;
        } else {
            throw std::invalid_argument("unable to find boundary type " + boundary[d]);
        }
    }

    // Make copy with PetscInt
    std::vector<PetscInt> facesPetsc(faces.begin(), faces.end());
    DM dm;
    DMPlexCreateBoxMesh(PETSC_COMM_WORLD, dimensions, simplex ? PETSC_TRUE : PETSC_FALSE, &facesPetsc[0], &lower[0], &upper[0], &boundaryTypes[0], PETSC_TRUE, &dm) >> checkError;
    return dm;
}

#include "registrar.hpp"
REGISTER(ablate::domain::Domain, ablate::domain::BoxMesh, "simple uniform box mesh", ARG(std::string, "name", "the name of the domain/mesh object"),
         OPT(std::vector<ablate::domain::FieldDescriptor>, "fields", "a list of fields/field descriptors"),
         OPT(std::vector<ablate::domain::modifiers::Modifier>, "modifiers", "a list of domain modifier"), ARG(std::vector<int>, "faces", "the number of faces in each direction"),
         ARG(std::vector<double>, "lower", "the lower bound of the mesh"), ARG(std::vector<double>, "upper", "the upper bound of the mesh"),
         OPT(std::vector<std::string>, "boundary", "custom boundary types (NONE, GHOSTED, MIRROR, PERIODIC)"), OPT(bool, "simplex", "sets if the elements/cells are simplex"));