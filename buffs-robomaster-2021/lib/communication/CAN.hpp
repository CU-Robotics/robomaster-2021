namespace buffs-robomaster-2021::lib::communication {
    enum class CANBus {
        BUS_1,
        BUS_2
    }

    enum class CANStatus {
        IDLE,
        BUSY,
        ERROR
    }

    class CAN {
        CAN();
        CANStatus status();
        bool getMessage();
    }
}