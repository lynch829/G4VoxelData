//////////////////////////////////////////////////////////////////////////
// G4VoxelData
// ===========
// A general interface for loading voxelised data as geometry in GEANT4.
//
// Author:  Christopher M Poole <mail@christopherpoole.net>
// Source:  http://github.com/christopherpoole/G4VoxelData
//
// License & Copyright
// ===================
// 
// Copyright 2013 Christopher M Poole <mail@christopherpoole.net>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////////


#include <iostream>

// USER //
#include "DetectorConstruction.hh"

// GEANT4 //
#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4UserLimits.hh"
#include "G4UIcommand.hh"


// G4VoxelData //
#include "G4VoxelDataParameterisation.hh"
#include "HDF5MappedIO.hh"


DetectorConstruction::DetectorConstruction(G4String filename)
{
    this->filename = filename;
    
    disk_array = new HDF5MappedIO<int>(); 
}


DetectorConstruction::~DetectorConstruction()
{
}


G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager* nist_manager = G4NistManager::Instance();
    G4Material* air = nist_manager->FindOrBuildMaterial("G4_AIR");
    G4Material* water = nist_manager->FindOrBuildMaterial("G4_WATER");

    world_solid = new G4Box("world_solid", 200*cm, 200*cm, 200*cm);
    world_logical = new G4LogicalVolume(world_solid, air, "world_logical", 0, 0, 0);
    world_physical = new G4PVPlacement(0, G4ThreeVector(), world_logical,
            "world_physical", 0, false, 0);
    world_logical->SetVisAttributes(G4VisAttributes::Invisible);

    disk_array->Read(filename.c_str(), "data");

    std::vector<double> spacing;
    for (unsigned int i=0; i<disk_array->GetDimensions(); i++) spacing.push_back(1.);
    disk_array->SetSpacing(spacing);

    std::vector<unsigned int> shape = disk_array->GetShape();

    // Crop if desired, array->Crop(xmin, xmax, ymin, ymax, zmin, zmax);
    //disk_array->Crop(0, shape[0], 0, shape[1], 0, shape[2]);

    std::map<int, G4Material*> materials;
    std::map<int, G4Colour*> colours;
    for (int i=0; i<256; i++) {
        materials[i] = water;
    
        double gray = (double) i / 255.;
        colours[i] = new G4Colour(gray, gray, gray, 1);
    }
    
    // The first template param is for the Array, second is for the map.
    G4VoxelDataParameterisation<int>* voxeldata_param =
        new G4VoxelDataParameterisation<int>(disk_array, materials,
                                                                world_physical);
    
    voxeldata_param->SetColourMap(colours);

    G4RotationMatrix* rot = new G4RotationMatrix;
    //rot->rotateZ(90*deg);
    voxeldata_param->Construct(G4ThreeVector(), rot);

    // Setup scoring with bins that are doubles
    //scorer = new G4VoxelDetector<double>("detector", array->GetShape(), array->GetSpacing());
    //scorer->SetDebug(true);

    //G4SDManager* sensitive_detector_manager = G4SDManager::GetSDMpointer();
    //sensitive_detector_manager->AddNewDetector(scorer);
    //voxeldata_param->GetLogicalVolume()->SetSensitiveDetector(scorer);

    return world_physical;
}

