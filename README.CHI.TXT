# 
# ./do has completed successfully
#
# to continue follow the below instructions
#

cd build/i2p/

# intialize database and import existing netdb
# requires python 3
./i2pinit.py --port $port --netdb $i2p_netDb_folder

# run i2p
# requires you to manually port forward $port on your router
./i2p
