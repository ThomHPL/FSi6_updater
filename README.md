# FSi6_updater
custom tools and updater for the FlySky i6 transmitter

    [===============================================]
    |    FLYSKY FS-i6 CUSTOM FIRMWARE UPLOADER      |
    |    by Thom                                    |
    |                                               |
    |    check out my blog for more:                |
    |    basejunction.wordpress.com                 |
    [===============================================]


!!! Please make sure the firmware file comes from basejunction.wordpress.com !!!

Install:

Place your transmitter in firmware update mode, then launch run.bat.
Type:
>> open 1 115200 (replace the 1 with your com port number)

>> reset (to test the communication)

Place your TX in firmware update mode again.
Type:
>> flash patch2.bin

wait till page 0xF300, if your TX restart on its own, the update should be properly done
