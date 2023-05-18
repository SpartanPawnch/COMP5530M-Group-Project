# Install script for directory: C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/compiler/cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/BuildSys")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include/windows" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/windows/PsWindowsAoS.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/windows/PsWindowsFPU.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/windows/PsWindowsInclude.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/windows/PsWindowsInlineAoS.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/windows/PsWindowsIntrinsics.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/windows/PsWindowsTrigConstants.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/BuildSys/include/foundation/windows/PxWindowsIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/BuildSys/include/foundation/windows" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/windows/PxWindowsIntrinsics.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/BuildSys/include/foundation/unix/PxUnixIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/BuildSys/include/foundation/unix" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/unix/PxUnixIntrinsics.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXFoundation_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXFoundation_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXFoundation_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXFoundation_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxFoundation.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/foundation" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/foundation/PxAssert.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/foundation/PxFoundationConfig.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/foundation/PxMathUtils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/Ps.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsAlignedMalloc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsAlloca.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsAllocator.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsAoS.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsArray.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsAtomic.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsBasicTemplates.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsBitUtils.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsBroadcast.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsCpu.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsFoundation.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsFPU.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsHash.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsHashInternals.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsHashMap.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsHashSet.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsInlineAllocator.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsInlineAoS.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsInlineArray.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsIntrinsics.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsMathUtils.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsMutex.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsPool.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsSList.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsSocket.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsSort.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsSortInternals.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsString.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsSync.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsTempAllocator.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsThread.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsTime.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsUserAllocated.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsUtilities.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsVecMath.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsVecMathAoSScalar.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsVecMathAoSScalarInline.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsVecMathSSE.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsVecMathUtilities.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsVecQuat.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/foundation/include/PsVecTransform.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/BuildSys/include/foundation/Px.h;C:/Program Files (x86)/BuildSys/include/foundation/PxAllocatorCallback.h;C:/Program Files (x86)/BuildSys/include/foundation/PxProfiler.h;C:/Program Files (x86)/BuildSys/include/foundation/PxSharedAssert.h;C:/Program Files (x86)/BuildSys/include/foundation/PxBitAndData.h;C:/Program Files (x86)/BuildSys/include/foundation/PxBounds3.h;C:/Program Files (x86)/BuildSys/include/foundation/PxErrorCallback.h;C:/Program Files (x86)/BuildSys/include/foundation/PxErrors.h;C:/Program Files (x86)/BuildSys/include/foundation/PxFlags.h;C:/Program Files (x86)/BuildSys/include/foundation/PxIntrinsics.h;C:/Program Files (x86)/BuildSys/include/foundation/PxIO.h;C:/Program Files (x86)/BuildSys/include/foundation/PxMat33.h;C:/Program Files (x86)/BuildSys/include/foundation/PxMat44.h;C:/Program Files (x86)/BuildSys/include/foundation/PxMath.h;C:/Program Files (x86)/BuildSys/include/foundation/PxMemory.h;C:/Program Files (x86)/BuildSys/include/foundation/PxPlane.h;C:/Program Files (x86)/BuildSys/include/foundation/PxPreprocessor.h;C:/Program Files (x86)/BuildSys/include/foundation/PxQuat.h;C:/Program Files (x86)/BuildSys/include/foundation/PxSimpleTypes.h;C:/Program Files (x86)/BuildSys/include/foundation/PxStrideIterator.h;C:/Program Files (x86)/BuildSys/include/foundation/PxTransform.h;C:/Program Files (x86)/BuildSys/include/foundation/PxUnionCast.h;C:/Program Files (x86)/BuildSys/include/foundation/PxVec2.h;C:/Program Files (x86)/BuildSys/include/foundation/PxVec3.h;C:/Program Files (x86)/BuildSys/include/foundation/PxVec4.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/BuildSys/include/foundation" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/Px.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxAllocatorCallback.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxProfiler.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxSharedAssert.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxBitAndData.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxBounds3.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxErrorCallback.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxErrors.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxFlags.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxIntrinsics.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxIO.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxMat33.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxMat44.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxMath.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxMemory.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxPlane.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxPreprocessor.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxQuat.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxSimpleTypes.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxStrideIterator.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxTransform.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxUnionCast.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxVec2.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxVec3.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/../pxshared/include/foundation/PxVec4.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gpu" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/gpu/PxGpu.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cudamanager" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cudamanager/PxCudaContextManager.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cudamanager/PxCudaMemoryManager.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common/windows" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/windows/PxWindowsDelayLoadHook.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysX_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysX_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysX_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysX_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxActor.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxAggregate.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxArticulationReducedCoordinate.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxArticulationBase.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxArticulation.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxArticulationJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxArticulationJointReducedCoordinate.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxArticulationLink.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxBatchQuery.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxBatchQueryDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxBroadPhase.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxClient.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxConstraint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxConstraintDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxContact.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxContactModifyCallback.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxDeletionListener.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxFiltering.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxForceMode.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxImmediateMode.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxLockedData.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxMaterial.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxPhysics.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxPhysicsAPI.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxPhysicsSerialization.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxPhysicsVersion.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxPhysXConfig.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxPruningStructure.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxQueryFiltering.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxQueryReport.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxRigidActor.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxRigidBody.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxRigidDynamic.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxRigidStatic.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxScene.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxSceneDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxSceneLock.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxShape.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxSimulationEventCallback.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxSimulationStatistics.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxVisualizationParameter.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxBase.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxCollection.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxCoreUtilityTypes.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxMetaData.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxMetaDataFlags.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxPhysicsInsertionCallback.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxPhysXCommonConfig.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxRenderBuffer.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxSerialFramework.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxSerializer.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxStringTable.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxTolerancesScale.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxTypeInfo.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/common/PxProfileZone.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pvd" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/pvd/PxPvdSceneClient.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/pvd/PxPvd.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/pvd/PxPvdTransport.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/collision" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/collision/PxCollisionDefs.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/solver" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/solver/PxSolverDefs.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/PxConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCharacterKinematic_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCharacterKinematic_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCharacterKinematic_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCharacterKinematic_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/characterkinematic" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/characterkinematic/PxBoxController.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/characterkinematic/PxCapsuleController.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/characterkinematic/PxController.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/characterkinematic/PxControllerBehavior.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/characterkinematic/PxControllerManager.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/characterkinematic/PxControllerObstacles.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/characterkinematic/PxExtended.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCommon_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCommon_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCommon_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCommon_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geometry" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxBoxGeometry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxCapsuleGeometry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxConvexMesh.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxConvexMeshGeometry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxGeometry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxGeometryHelpers.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxGeometryQuery.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxHeightField.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxHeightFieldDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxHeightFieldFlag.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxHeightFieldGeometry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxHeightFieldSample.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxMeshQuery.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxMeshScale.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxPlaneGeometry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxSimpleTriangleMesh.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxSphereGeometry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxTriangle.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxTriangleMesh.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxTriangleMeshGeometry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geometry/PxBVHStructure.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geomutils" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geomutils/GuContactBuffer.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/geomutils/GuContactPoint.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXCooking_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXCooking_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXCooking_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXCooking_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cooking" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cooking/PxBVH33MidphaseDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cooking/PxBVH34MidphaseDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cooking/Pxc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cooking/PxConvexMeshDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cooking/PxCooking.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cooking/PxMidphaseDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cooking/PxTriangleMeshDesc.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/cooking/PxBVHStructureDesc.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXExtensions_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXExtensions_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXExtensions_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXExtensions_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/extensions" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxBinaryConverter.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxBroadPhaseExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxCollectionExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxConstraintExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxContactJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxConvexMeshExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxD6Joint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxD6JointCreate.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxDefaultAllocator.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxDefaultCpuDispatcher.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxDefaultErrorCallback.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxDefaultSimulationFilterShader.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxDefaultStreams.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxDistanceJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxContactJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxExtensionsAPI.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxFixedJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxJointLimit.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxMassProperties.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxPrismaticJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxRaycastCCD.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxRepXSerializer.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxRepXSimpleType.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxRevoluteJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxRigidActorExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxRigidBodyExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxSceneQueryExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxSerialization.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxShapeExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxSimpleFactory.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxSmoothNormals.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxSphericalJoint.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxStringTableExt.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/extensions/PxTriangleMeshExt.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/filebuf" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/filebuf/PxFileBuf.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXVehicle_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXVehicle_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXVehicle_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXVehicle_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleComponents.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleDrive.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleDrive4W.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleDriveNW.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleDriveTank.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleNoDrive.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleSDK.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleShaders.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleTireFriction.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleUpdate.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleUtil.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleUtilControl.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleUtilSetup.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleUtilTelemetry.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/vehicle/PxVehicleWheels.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/fastxml/include" TYPE FILE FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/source/fastxml/include/PsFastXml.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXPvdSDK_staticd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXPvdSDK_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXPvdSDK_static.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXPvdSDK_static.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/debug/PhysXTaskd.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/checked/PhysXTask.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/profile/PhysXTask.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE FILE OPTIONAL FILES "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/bin/win.x86_64.vc142.md/release/PhysXTask.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/task" TYPE FILE FILES
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/task/PxCpuDispatcher.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/task/PxTask.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/task/PxTaskDefine.h"
    "C:/Users/marti/Documents/BuildSystemConcept/external/physx-4.1/physx/include/task/PxTaskManager.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/debug/PhysXFoundation_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/checked/PhysXFoundation_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/profile/PhysXFoundation_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/release/PhysXFoundation_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/debug/PhysX_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/checked/PhysX_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/profile/PhysX_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/release/PhysX_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/debug/PhysXCharacterKinematic_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/checked/PhysXCharacterKinematic_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/profile/PhysXCharacterKinematic_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/release/PhysXCharacterKinematic_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/debug/PhysXPvdSDK_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/checked/PhysXPvdSDK_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/profile/PhysXPvdSDK_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/release/PhysXPvdSDK_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/debug/PhysXCommon_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/checked/PhysXCommon_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/profile/PhysXCommon_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/release/PhysXCommon_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/debug/PhysXCooking_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/checked/PhysXCooking_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/profile/PhysXCooking_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/release/PhysXCooking_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/debug/PhysXExtensions_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/checked/PhysXExtensions_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/profile/PhysXExtensions_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/release/PhysXExtensions_static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/debug" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/debug/PhysXVehicle_staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/checked" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/checked/PhysXVehicle_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/profile" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/profile/PhysXVehicle_static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.md/release" TYPE STATIC_LIBRARY FILES "C:/Users/marti/Documents/BuildSystemConcept/externals/physx/bin/win.x86_64.vc142.md/release/PhysXVehicle_static.lib")
  endif()
endif()

