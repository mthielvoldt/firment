
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


add_custom_command(
  OUTPUT ${VIRTUAL_ENV_DIR}
  COMMAND python3 -m venv "${VIRTUAL_ENV_DIR}" &&
    ${VIRTUAL_ENV_ACTIVATE} &&
    python -m pip install ${PYTHON_PACKAGES}
)
