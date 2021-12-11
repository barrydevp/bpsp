package com.ui;

import javax.swing.*;
import java.awt.*;

public abstract class DevicePanel extends JPanel {

    public enum DeviceType {
        ENERGY_TRACKER("Energy Tracker"),
        TEMPERATURE_TRACKER("Temperature Tracker"),
        CONNECTOR("Connector"),
        SPEAKER("Speak"),
        POWER("Power"),
        UNKNOWN("Unknown");

        private final String typeStr;

        DeviceType(String typeStr) {
            this.typeStr = typeStr;
        }

        public String getTypeStr() {
            return typeStr;
        }

        static DeviceType getType(String type) {
            if (type.equals(ENERGY_TRACKER.getTypeStr())) {
                return ENERGY_TRACKER;
            } else if (type.equals(TEMPERATURE_TRACKER.getTypeStr())) {
                return TEMPERATURE_TRACKER;
            } else if (type.equals(CONNECTOR.getTypeStr())) {
                return CONNECTOR;
            } else if (type.equals(SPEAKER.getTypeStr())) {
                return SPEAKER;
            } else if (type.equals(POWER.getTypeStr())) {
                return POWER;
            } else {
                return UNKNOWN;
            }
        }

    }

    static DevicePanel createDevicePanel(SmartHomeUI.Device device) {
        switch (device.getType()) {
            case ENERGY_TRACKER:
            case TEMPERATURE_TRACKER:
            case CONNECTOR:
            case SPEAKER:
            case POWER:
            default:
                return new EnergyTracker(device);
        }
    }

    static final int MAX_HEIGHT = 60;

    // Data
    SmartHomeUI.Device device;

    // UI
    JLabel nameLabel = new JLabel("Unknown");
    JLabel typeLabel = new JLabel("Unknown");
    JLabel dataLabel = new JLabel("Unknown");

    DevicePanel(SmartHomeUI.Device device) {
        this.device = device;

        this.setPreferredSize(new Dimension(
                (SmartHomeUI.MonitorPanel.ROOM_CONTAINER_WIDTH - 20) / 2,
                (SmartHomeUI.MonitorPanel.ROOM_CONTAINER_HEIGHT - 20) / 2
        ));
        this.setSize(this.getPreferredSize());
        this.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createEmptyBorder(2, 2, 2, 2),
        BorderFactory.createMatteBorder(2, 2, 2, 2, Color.BLUE)
        ));

        nameLabel.setText(device.getName());
        typeLabel.setText(device.getType().getTypeStr());
        dataLabel.setText(device.getData());

        this.add(nameLabel);
        this.add(typeLabel);
        this.add(dataLabel);

        this.setVisible(true);
    }

    public void reRender() {
        nameLabel.setText(device.getName());
        typeLabel.setText(device.getType().getTypeStr());
        dataLabel.setText(device.getData());

        this.repaint();
        this.revalidate();
    }
}

class EnergyTracker extends DevicePanel {

    EnergyTracker(SmartHomeUI.Device device) {
        super(device);
    }
}
