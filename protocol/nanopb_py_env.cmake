
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(PYTHON_PACKAGES
  "grpcio-tools"
)

set(VIRTUAL_ENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/venv")
if(WIN32)
  set(VIRTUAL_ENV_ACTIVATE "${VIRTUAL_ENV_DIR}/Scripts/activate")
else()
  set(VIRTUAL_ENV_ACTIVATE . ${VIRTUAL_ENV_DIR}/bin/activate)
endif()

# Create the virtual environment and pip install python packages. 
if(NOT EXISTS "${VIRTUAL_ENV_DIR}")
  message(STATUS "Creating virtual environment: ${VIRTUAL_ENV_DIR}")
  execute_process(
    COMMAND ${Python3_EXECUTABLE} -m venv "${VIRTUAL_ENV_DIR}" 
    RESULT_VARIABLE result
  )
  if(result)
      message(FATAL_ERROR "Failed to create virtual environment.")
  endif()
endif()


# Create a custom target for installing dependencies. This is important so the dependencies are installed before any other target that needs them.
add_custom_target(install_python_dependencies
    COMMAND ${VIRTUAL_ENV_ACTIVATE} && python -m pip install ${PYTHON_PACKAGES} # Combined command
    DEPENDS ${VIRTUAL_ENV_DIR} # Ensure the virtual env exists first
    COMMENT "Installing Python dependencies"
)
