package com.ui;

import com.utils.SystemUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.util.Locale;

public abstract class DevicePanel extends JPanel {
    private static final Logger LOGGER = LogManager.getLogger(DevicePanel.class);

    public enum DeviceType {
        LIGHT("Light"),
        THERMOSTAT("Thermostat"),
        FRIDGE("Fridge"),
        SPEAKER("Speaker"),
        FANS("Fans"),
        TV("TV"),
        COMPUTER("Computer"),
        CUSTOM("Custom");

        static String[] listStr = new String[]{LIGHT.typeStr, THERMOSTAT.typeStr, TV.typeStr, COMPUTER.typeStr, FRIDGE.typeStr, SPEAKER.typeStr, FANS.typeStr};

        private final String typeStr;
        private final ImageIcon icon;

        DeviceType(String typeStr) {
            this.typeStr = typeStr;
            this.icon = new ImageIcon(
                    (new ImageIcon(SystemUtils.getImagePath(typeStr.toLowerCase(Locale.ROOT) + "-type.png")))
                            .getImage()
                            .getScaledInstance(32, 32, Image.SCALE_SMOOTH)
            );
//            this.icon = new ImageIcon(SystemUtils.getImagePath(typeStr.toLowerCase(Locale.ROOT) + "-type.png"));
        }

        public String getTypeStr() {
            return typeStr;
        }

        public String toString() {
            return typeStr;
        }

        public ImageIcon getIcon() {
            return icon;
        }

        static DeviceType getType(String type) {
            if (type.equals(LIGHT.getTypeStr())) {
                return LIGHT;
            } else if (type.equals(THERMOSTAT.getTypeStr())) {
                return THERMOSTAT;
            } else if (type.equals(FRIDGE.getTypeStr())) {
                return FRIDGE;
            } else if (type.equals(SPEAKER.getTypeStr())) {
                return SPEAKER;
            } else if (type.equals(FANS.getTypeStr())) {
                return FANS;
            } else if (type.equals(TV.getTypeStr())) {
                return TV;
            } else if (type.equals(COMPUTER.getTypeStr())) {
                return COMPUTER;
            } else {
                return CUSTOM;
            }
        }

        static ImageIcon getIcon(DeviceType type) {
            ImageIcon icon;
            switch (type) {
                case LIGHT:
                    icon = new ImageIcon(SystemUtils.getImagePath("light-type.png"));
                    break;
                case THERMOSTAT:
                    icon = new ImageIcon(SystemUtils.getImagePath("thermostat2-type.png"));
                    break;
                case FRIDGE:
                    icon = new ImageIcon(SystemUtils.getImagePath("fridge-type.png"));
                    break;
                case SPEAKER:
                    icon = new ImageIcon(SystemUtils.getImagePath("speaker-type.png"));
                    break;
                case FANS:
                    icon = new ImageIcon(SystemUtils.getImagePath("fans-type.png"));
                    break;
                default:
                    icon = new ImageIcon(SystemUtils.getImagePath("custom-type.png"));
            }

            return icon;
        }

        public static BufferedImage resize(BufferedImage image, int width, int height) {
            BufferedImage bi = new BufferedImage(width, height, BufferedImage.TRANSLUCENT);
            Graphics2D g2d = bi.createGraphics();
            g2d.addRenderingHints(new RenderingHints(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY));
            g2d.drawImage(image, 0, 0, width, height, null);
            g2d.dispose();
            return bi;
        }

        static String[] getListStr() {
            return listStr;
        }

    }

    static DevicePanel createDevicePanel(SmartHomeUI.Device device) {
        switch (device.getType()) {
            case LIGHT:
                return new LightDevice(device);
            case THERMOSTAT:
                return new ThermostatDevice(device);
            case FRIDGE:
                return new FridgeDevice(device);
            case SPEAKER:
                return new SpeakerDevice(device);
            case TV:
                return new TVDevice(device);
            case COMPUTER:
                return new ComputerDevice(device);
            case FANS:
                return new FansDevice(device);
            default:
                return new UnknownDevice(device);
        }
    }

    // Data
    SmartHomeUI.Device device;

    // UI
    Image backgroundImg;
    JLabel nameLabel = new JLabel("Unknown");
    JLabel typeLabel;
    JLabel dataLabel = new JLabel("Unknown");
    JButton removeBtn = new JButton("", createIconWithSize("cancel.png", 28, 28));

    ImageIcon onIcon = createIconWithSize("turn-on.png", 40, 20);
    ImageIcon offIcon = createIconWithSize("turn-off.png", 40, 20);
    JButton activeBtn = new JButton("", offIcon);

    public static ImageIcon createIconWithSize(String file, int width, int height) {
        return new ImageIcon((new ImageIcon(SystemUtils.getImagePath(file))).getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH));
    }

    GridBagConstraints c = new GridBagConstraints();

    DevicePanel(SmartHomeUI.Device device) {
        this.device = device;

        backgroundImg = (new ImageIcon(
                SystemUtils.getImagePath("device-card.png"))
        ).getImage();

        this.setLayout(new GridBagLayout());
        this.setPreferredSize(new Dimension(
                (SmartHomeUI.MonitorPanel.ROOM_CONTAINER_WIDTH - 20) / 2,
                (SmartHomeUI.MonitorPanel.ROOM_CONTAINER_HEIGHT - 20) / 2
        ));
        this.setSize(this.getPreferredSize());
        this.setBorder(BorderFactory.createEmptyBorder(10, 20, 10, 20));
//        this.setBorder(BorderFactory.createCompoundBorder(
//                BorderFactory.createEmptyBorder(2, 2, 2, 2),
//                BorderFactory.createCompoundBorder(
//                        BorderFactory.createMatteBorder(2, 2, 2, 2, Color.BLUE),
//                        BorderFactory.createLineBorder(Color.BLUE, 3, true)
//                )
//                BorderFactory.createLineBorder(Color.BLUE, 3, true)
//        ));

        typeLabel = new JLabel(device.getType().getIcon(), JLabel.LEADING);
//        typeLabel.setSize(48, 48);
        typeLabel.setText(device.getType().getTypeStr());
        typeLabel.setFont(new Font("Helvetica", Font.ITALIC, 12));
        typeLabel.setForeground(Color.DARK_GRAY);
        this.addToGrid(typeLabel, 0, 0, 1, 1, 1, 1, GridBagConstraints.BOTH);

        removeBtn.addActionListener((event) -> {
            device.getRoom().removeDevice(device);
        });
        removeBtn.setPreferredSize(new Dimension(30, 30));
        removeBtn.setBorder(BorderFactory.createEmptyBorder());
        removeBtn.setOpaque(false);
        this.addToGrid(removeBtn, 1, 0, 0.1, 0.1, 1, 1, GridBagConstraints.PAGE_END);

        activeBtn.addActionListener((event) -> {
            try {
                if (!device.isRunning()) {
                    device.start();
                    activeBtn.setIcon(onIcon);
                } else {
                    device.stop();
                    activeBtn.setIcon(offIcon);
                }
            } catch (Exception e) {
                LOGGER.error(e);
                JOptionPane.showMessageDialog(
                        this,
                        e.getMessage(),
                        "New Device Error",
                        JOptionPane.ERROR_MESSAGE
                );
            }
        });
        if (device.isRunning()) {
            activeBtn.setIcon(onIcon);
        }
        activeBtn.setPreferredSize(new Dimension(40, 20));
        activeBtn.setBorder(BorderFactory.createEmptyBorder());
        activeBtn.setOpaque(false);
        this.addToGrid(activeBtn, 1, 1, 0.3, 1, 1, 1, GridBagConstraints.PAGE_END);

        nameLabel.setText(device.getName());
        nameLabel.setFont(new Font("Helvetica", Font.BOLD, 18));
        this.addToGrid(nameLabel, 0, 1, 0.7, 1, 1, 1, GridBagConstraints.BOTH);

        dataLabel.setText("No data");
        dataLabel.setFont(new Font("Helvetica", Font.PLAIN, 16));
        dataLabel.setForeground(Color.GRAY);
        this.addToGrid(dataLabel, 0, 2, 1, 1, 1, 1, GridBagConstraints.BOTH);

        this.setVisible(true);
    }

    void addToGrid(
            Component comp,
            int gridX, int gridY,
            double weightX, double weightY,
            int gridWidth, int gridHeight,
            int fill
    ) {

        c.gridx = gridX;
        c.gridy = gridY;
        c.weightx = weightX;
        c.weighty = weightY;
        c.gridheight = gridHeight;
        c.gridwidth = gridWidth;
        c.fill = fill;
        this.add(comp, c);
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);// clear and repaint

        g.drawImage(this.backgroundImg, 0, 0, getWidth(), getHeight(), this);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}

class LightDevice extends DevicePanel {

    LightDevice(SmartHomeUI.Device device) {
        super(device);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}

class ThermostatDevice extends DevicePanel {

    ThermostatDevice(SmartHomeUI.Device device) {
        super(device);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}

class FridgeDevice extends DevicePanel {

    FridgeDevice(SmartHomeUI.Device device) {
        super(device);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}

class SpeakerDevice extends DevicePanel {

    SpeakerDevice(SmartHomeUI.Device device) {
        super(device);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}

class TVDevice extends DevicePanel {

    TVDevice(SmartHomeUI.Device device) {
        super(device);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}

class ComputerDevice extends DevicePanel {

    ComputerDevice(SmartHomeUI.Device device) {
        super(device);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}

class FansDevice extends DevicePanel {

    FansDevice(SmartHomeUI.Device device) {
        super(device);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}

class UnknownDevice extends DevicePanel {

    UnknownDevice(SmartHomeUI.Device device) {
        super(device);
    }

    public void reRender() {
        dataLabel.setText(device.getData() != null ? device.getData().getStrData() : "No data");

        this.repaint();
        this.revalidate();
    }
}
