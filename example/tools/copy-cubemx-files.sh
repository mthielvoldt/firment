
if [ ! -d $1 ]; then
  echo "Error: $1 is not a directory.  Please provide CubeMX output dir."
fi

CONF_H=$1/Inc/stm32l4xx_hal_conf.h


if [ ! -f $CONF_H ]; then
  echo "Error: $CONF_H not found"
else
  cp $CONF_H ../firmware/config/pcb1/
  echo "copied $CONF_H to pcb1"
fi

