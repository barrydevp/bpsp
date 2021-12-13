package com.ui;

import com.core.client.BPSPClient;
import com.core.client.Subscriber;
import com.core.frame.Frame;
import com.utils.SystemUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.swing.*;
import java.awt.*;
import java.util.Collection;
import java.util.HashMap;

public class SmartHomeUI extends AbstractUI {
    private static final Logger LOGGER = LogManager.getLogger(SmartHomeUI.class);

    // UI
    HeaderPanel headerPanel = new HeaderPanel("SMART HOME - BPSP Subscriber UI");
    ConnectPanel connectPanel = new ConnectPanel(this);
    MonitorPanel monitorPanel = new MonitorPanel();

    public SmartHomeUI() {
        this("Smart Home ^^!");
    }

    public SmartHomeUI(String title) {
        super(title);

        this.setLayout(new FlowLayout());
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, AbstractUI.MAX_HEIGHT));
        this.setSize(this.getPreferredSize());

        Container c = this.getContentPane();
//        c.setBackground(Color.WHITE);

        c.add(headerPanel);
        c.add(connectPanel);
        c.add(monitorPanel);

        this.setLocationRelativeTo(null);
        this.pack();
        this.setVisible(true);
    }

    @Override
    public void onConnected() {
        LOGGER.info("CONNECT UI.");
//        this.remove(connectPanel);
//        this.repaint();
//        this.revalidate();
        this.monitorPanel.open();
    }

    @Override
    public void onClosed() {
        LOGGER.info("CLOSE UI.");
        this.monitorPanel.close();
    }

    BPSPClient getBPSPClient() {
        return connectPanel.getClient();
    }

    class MonitorPanel extends JPanel {
        static final int BORDER_SIZE = 5;
        static final int MAX_HEIGHT = 380;
        static final int ROOM_CONTAINER_WIDTH = 400;
        static final int ROOM_CONTAINER_HEIGHT = 300;
        static final int LEFT_NAV_WIDTH = 150;

        // UI
        GridBagConstraints c = new GridBagConstraints();
        DefaultListModel<Room> listModel = new DefaultListModel<>();
        JList<Room> listRoom = new JList<>(listModel);
        NewDeviceFrame newDeviceFrame = new NewDeviceFrame();
        JButton newDeviceBtn = new JButton("New device");
        JPanel roomContainer;

        // Data
        RoomManager roomManager = new RoomManager();

        MonitorPanel() {
            this.setLayout(new GridBagLayout());
            this.setPreferredSize(new Dimension(AbstractUI.MAX_WIDTH, MAX_HEIGHT));
            this.setSize(this.getPreferredSize());
            this.setBorder(BorderFactory.createCompoundBorder(
                    BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE),
                    BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.PINK), "Home Monitoring")
            ));
//            this.setBackground(Color.WHITE);

            listRoom.setCellRenderer(new RoomRenderer());
            listRoom.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
            listRoom.setSelectedIndex(0);
            listRoom.addListSelectionListener(event -> {
                this.roomManager.selectRoom(listRoom.getSelectedValue());
            });

            newDeviceBtn.addActionListener(event -> {
                newDeviceFrame.open();
            });

            c.insets = new Insets(3, 3, 3, 3);

            JPanel controlPanel = new JPanel();
            controlPanel.setPreferredSize(new Dimension(LEFT_NAV_WIDTH, 50));
            controlPanel.add(newDeviceBtn);
            this.addToGrid(controlPanel, 0, 0, 0.3, 0.1, 1, 1, GridBagConstraints.BOTH);

            listRoom.setOpaque(false);
            JScrollPane listScroll = new JScrollPane(listRoom);
            listScroll.setOpaque(false);
            listScroll.getViewport().setOpaque(false);
            listScroll.setBorder(BorderFactory.createEmptyBorder());
            listScroll.setPreferredSize(new Dimension(LEFT_NAV_WIDTH, 300));
            listScroll.setSize(listScroll.getPreferredSize());
            this.addToGrid(listScroll, 0, 1, 0.3, 1, 1, 1, GridBagConstraints.BOTH);

            Image roomContainerBackground = (new ImageIcon(
                    SystemUtils.getImagePath("room-container.png"))
            ).getImage();
            roomContainer = new JPanel() {
                @Override
                public void paintComponent(Graphics g) {
                    super.paintComponent(g);// clear and repaint

                    g.drawImage(roomContainerBackground, 0, 0, getWidth(), getHeight(), this);
                }
            };
            roomContainer.setPreferredSize(new Dimension(ROOM_CONTAINER_WIDTH + 20, ROOM_CONTAINER_HEIGHT + 20));
            roomContainer.setSize(roomContainer.getPreferredSize());
//            roomContainer.setBorder(BorderFactory.createLineBorder(Color.ORANGE, 2, true));
            roomContainer.setVisible(true);

            this.addToGrid(roomContainer, 1, 0, 0.7, 1, 1, 2, GridBagConstraints.BOTH);

            this.setVisible(false);
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

        void open() {
            roomManager.clear();
            this.repaint();
            this.revalidate();
            this.setVisible(true);
        }

        void close() {
            this.setVisible(false);
            roomManager.clear();
            listModel.clear();
            listRoom.clearSelection();
        }

        void renderRoomList() {
            listModel.clear();

            for (Room room : roomManager.getRooms()) {
                listModel.addElement(room);
            }

            listRoom.setSelectedIndex(0);
        }

        void addRoomToRenderList(Room room) {
            if (!listModel.contains(room)) {
                listModel.addElement(room);
            }

//            this.listRoom.repaint();
//            this.listRoom.revalidate();
        }

        void removeRoomFromRenderList(Room room) {
            listModel.removeElement(room);
        }

        class NewDeviceFrame extends JFrame {
            static final int BORDER_SIZE = 20;
            static final int MAX_WIDTH = 400;
            static final int MAX_HEIGHT = 400;

            JPanel panel = new JPanel();
            JLabel roomLabel = new JLabel("Room: ");
            JComboBox<String> roomField = new JComboBox<>(new String[]{"locationC"});
            JLabel deviceLabel = new JLabel("Device Name: ");
            JTextField deviceField = new JTextField("");
            JLabel deviceTypeLabel = new JLabel("Device Type: ");
            JComboBox<DevicePanel.DeviceType> deviceTypeField = new JComboBox<>(DevicePanel.DeviceType.values());
            Button newBtn = new Button("New");

            NewDeviceFrame() {
                super("Add new device");

                panel.setLayout(new GridLayout(0, 2, 5, 1));
                panel.setBorder(BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE));
//                panel.setPreferredSize(new Dimension(NewDeviceFrame.MAX_WIDTH, NewDeviceFrame.MAX_HEIGHT));
//                panel.setSize(this.getPreferredSize());

//                roomField.setColumns(10);
                deviceField.setColumns(10);

                newBtn.addActionListener((event) -> {
                    String roomName = (String) roomField.getModel().getSelectedItem();
                    String deviceName = deviceField.getText();
                    DevicePanel.DeviceType deviceType = (DevicePanel.DeviceType) deviceTypeField.getModel().getSelectedItem();

                    if (deviceType == null || roomName == null || roomName.isEmpty() || deviceName.isEmpty()) {
                        JOptionPane.showMessageDialog(
                                this,
                                "Invalid input, required field.",
                                "Add error",
                                JOptionPane.ERROR_MESSAGE
                        );

                        return;
                    }

                    RoomManager roomManager = MonitorPanel.this.roomManager;
                    Device device = roomManager.addDevice(roomName, deviceName, deviceType.getTypeStr());

                    if (device == null) {
                        return;
                    }

//                    try {
//                        device.start();
//                        this.setVisible(false);
//                    } catch (Exception e) {
//                        LOGGER.error(e);
//                        JOptionPane.showMessageDialog(
//                                this,
//                                e.getMessage(),
//                                "New Device Error",
//                                JOptionPane.ERROR_MESSAGE
//                        );
//                    }

                    this.setVisible(false);
                });

                roomField.setEditable(true);
                panel.add(roomLabel);
                panel.add(roomField);
                panel.add(deviceTypeLabel);
                panel.add(deviceTypeField);
                panel.add(deviceLabel);
                panel.add(deviceField);
                panel.add(newBtn);

                Container c = this.getContentPane();
                c.add(panel);

                // current frame commit
//            this.setLocationRelativeTo(SmartHomeUI.this);
                this.setLocationRelativeTo(null);
                this.pack();
                this.setVisible(false);
            }

            void renderRoomList() {
                this.roomField.removeAllItems();
                for (Room room : MonitorPanel.this.roomManager.getRooms()) {
                    this.roomField.addItem(room.getName());
                }
            }

            void open() {
                deviceField.setText("");
                this.renderRoomList();
                this.repaint();
                this.invalidate();
                this.setVisible(true);
            }

            void close() {
                this.setVisible(false);
            }
        }

        class RoomManager {
            HashMap<String, Room> rooms = new HashMap<>();
            Room currentRoom = null;

            void selectRoom(Room room) {
                if (currentRoom == room) {
                    return;
                }

                if (currentRoom != null) {
                    currentRoom.unmount(MonitorPanel.this.roomContainer);
                }

                currentRoom = room;

                // render to UI
                if (room != null) {
                    room.render(MonitorPanel.this.roomContainer);
                }
            }

            Room getCurrentRoom() {
                return this.currentRoom;
            }

            Collection<Room> getRooms() {
                return this.rooms.values();
            }

            Room getRoom(String roomName) {
                Room room = this.rooms.get(roomName);

                return room;
            }

            Room addRoom(String roomName) {
                Room room = new Room(roomName);
                this.rooms.put(roomName, room);

                // add to UI
                MonitorPanel.this.addRoomToRenderList(room);

                return room;
            }

            Device addDevice(String roomName, String deviceName, String deviceType) {
                Room room = this.getRoom(roomName);

                if (room == null) {
                    room = this.addRoom(roomName);
                }

                return room.addDevice(deviceName, deviceType);
            }

            Device getDevice(String roomName, String deviceName) {
                Room room = this.getRoom(roomName);

                if (room == null) {
                    return null;
                }

                return room.getDevice(deviceName);
            }

            void removeRoom(String roomName) {
                Room room = this.getRoom(roomName);

                this.removeRoom(room);
            }

            void removeRoom(Room room) {
                if (room != null) {
                    // remove from UI
                    MonitorPanel.this.removeRoomFromRenderList(room);

                    if (this.currentRoom == room) {
                        room.unmount(MonitorPanel.this.roomContainer);
                    }

                    room.clear();
                    rooms.remove(room.getName());
                }
            }

            void clear() {
                if (SmartHomeUI.this.getBPSPClient().isConnected()) {
                    for (Room room : rooms.values()) {
                        room.clear();
                    }
                }

                rooms.clear();
            }
        }

        public class RoomRenderer implements ListCellRenderer<Room> {

            public RoomRenderer() {
            }

            @Override
            public Component getListCellRendererComponent(JList<? extends Room> list, Room value, int index, boolean isSelected, boolean cellHasFocus) {
                //Get the selected index. (The index param isn't
                //always valid, so just use the value.)

                JPanel panel = value.getItemPanel();

                value.itemLabel.setText(value.getName());
                value.numsDeviceLabel.setText(value.devices.size() + " Devices");
//            label.setOpaque(true);

                // when select item
                if (isSelected) {
//                    MonitorPanel.this.newDeviceFrame.roomField.setSelectedItem();
                    value.isActive = true;
//                label.setBackground(list.getSelectionBackground());
//                panel.setBackground(list.getSelectionBackground());
                } else { // when don't select
                    value.isActive = false;
//                label.setBackground(list.getBackground());
//                panel.setBackground(list.getBackground());
                }

                return panel;
            }

        }

    }

    class Room {
        // UI
        JPanel panel;
        JScrollPane container;
        GridBagConstraints c = new GridBagConstraints();
        // for RenderList
        boolean isActive = false;
        JPanel itemPanel;
        JLabel itemLabel = new JLabel();
        JLabel numsDeviceLabel = new JLabel();
        JButton itemRemoveBtn = new JButton(DevicePanel.createIconWithSize("cancel.png", 20, 20));
        JLabel roomIconLb = new JLabel(DevicePanel.createIconWithSize("room-icon.png", 35, 35));

        // Data
        String name;
        HashMap<String, Device> devices = new HashMap<>();

        Room(String name) {
            this.name = name;

            panel = new JPanel();
            panel.setLayout(new GridBagLayout());
            panel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
            container = new JScrollPane(panel);
            container.setBorder(BorderFactory.createEmptyBorder());
            container.setPreferredSize(new Dimension(
                    MonitorPanel.ROOM_CONTAINER_WIDTH + 10,
                    MonitorPanel.ROOM_CONTAINER_HEIGHT + 10)
            );
            container.setSize(panel.getPreferredSize());

            c.gridx = 0;
            c.gridy = 0;
            c.gridwidth = 1;
            c.gridheight = 1;

            panel.setVisible(true);

            // for render item list
            Image roomItemBg = (new ImageIcon(
                    SystemUtils.getImagePath("room-item-bg.png"))
            ).getImage();
            Image roomItemBgActive = (new ImageIcon(
                    SystemUtils.getImagePath("room-item-bg-active.png"))
            ).getImage();
            itemPanel = new JPanel() {
                @Override
                public void paintComponent(Graphics g) {
                    super.paintComponent(g);// clear and repaint

                    g.drawImage(isActive ? roomItemBgActive : roomItemBg, 0, 0, getWidth(), getHeight(), this);
                }
            };
            itemPanel.setLayout(new BorderLayout(5, 20));
            itemPanel.setBorder(BorderFactory.createEmptyBorder(5, 20, 5, 60));
            itemPanel.setPreferredSize(new Dimension(MonitorPanel.LEFT_NAV_WIDTH - 20, 50));
            itemPanel.setSize(itemPanel.getPreferredSize());

            // remove button
//            itemRemoveBtn.setBorder(BorderFactory.createEmptyBorder());
//            itemRemoveBtn.setPreferredSize(new Dimension(20, 20));
//            itemRemoveBtn.addActionListener((event) -> {
//                SmartHomeUI.this.monitorPanel.roomManager.removeRoom(this);
//            });
//            itemPanel.add(itemRemoveBtn, BorderLayout.WEST);
            itemPanel.add(roomIconLb, BorderLayout.WEST);

            // label text
            itemLabel.setFont(new Font("Helvetica", Font.PLAIN, 15));
            itemPanel.add(itemLabel, BorderLayout.CENTER);
            numsDeviceLabel.setFont(new Font("Helvetica", Font.PLAIN, 13));
            numsDeviceLabel.setForeground(Color.GRAY);
            itemPanel.add(numsDeviceLabel, BorderLayout.EAST);
        }

        public JPanel getItemPanel() {
            return itemPanel;
        }

        void render(JPanel container) {
            container.add(this.container);
            container.repaint();
            container.revalidate();
        }

        void unmount(JPanel container) {
            container.remove(this.container);
            container.repaint();
            container.revalidate();
        }

        Device addDevice(String deviceName, String type) {
            return this.addDevice(new Device(this, deviceName, type));
        }

        Device addDevice(Device device) {
            Device old = this.devices.putIfAbsent(device.getName(), device);

            if (old == null) {
                old = device;
                this.addDevicePanel(device, true);
//                if(this.container != null) {
//                    this.container.repaint();
//                    this.container.revalidate();
//                }
            }

            return old;
        }

        void addDevicePanel(Device device, boolean repaint) {
            panel.add(device.getPanel(), c);
            if (c.gridx == 1) {
                c.gridx = 0;
                c.gridy++;
            } else {
                c.gridx++;
            }

            if (repaint) {
                panel.repaint();
                panel.revalidate();
            }
        }

        void reRender() {
            c.gridx = 0;
            c.gridy = 0;
            c.gridwidth = 1;
            c.gridheight = 1;
            panel.removeAll();

            for (Device device : devices.values()) {
                this.addDevicePanel(device, false);
            }

            panel.repaint();
            panel.validate();
        }

        public String getName() {
            return name;
        }

        public Device getDevice(String deviceName) {
            return this.devices.get(deviceName);
        }

        void clear() {
            for (Device device : devices.values()) {
                try {
                    device.stop();
                } catch (Exception e) {
                    LOGGER.error(e);
                }
            }

            devices.clear();
        }

        void removeDevice(Device device) {
            try {
                device.stop();
            } catch (Exception e) {
                LOGGER.error(e);
            }

            devices.remove(device.getName());
            this.reRender();
//            panel.remove(device.getPanel());
//            panel.repaint();
//            panel.revalidate();
        }

        public String toString() {
            return this.name + " (" + this.devices.size() + ") devices";
        }

    }

    public class Device {
        // UI
        final DevicePanel panel;

        // data
        final Room room;
        final String name;
        final DevicePanel.DeviceType type;
        Frame data;
        boolean running = false;
        Subscriber subscriber;

        Device(Room room, String name, String type) {
            this.room = room;
            this.name = name;
            this.type = DevicePanel.DeviceType.getType(type);
            this.panel = DevicePanel.createDevicePanel(this);

            this.subscriber = new DeviceSubscriber(room.getName() + "/" + name);
        }

        public Room getRoom() {
            return room;
        }

        public boolean isRunning() {
            return running;
        }

        public DevicePanel getPanel() {
            return this.panel;
        }

        public String getName() {
            return name;
        }

        public DevicePanel.DeviceType getType() {
            return type;
        }

        public void start() throws Exception {
            if (!running) {
                SmartHomeUI.this.getBPSPClient().sub(this.subscriber);
                running = true;
            }
        }

        public void stop() throws Exception {
            if (running) {
                SmartHomeUI.this.getBPSPClient().unsub(this.subscriber);
                running = false;
            }
        }

        public void setData(Frame data) {
            this.data = data;
        }

        public Frame getData() {
            return this.data;
        }

        class DeviceSubscriber extends Subscriber {
            DeviceSubscriber(String topic) {
                super(SmartHomeUI.this.getBPSPClient(), topic);
            }

            @Override
            public void consumeMsg(Frame msg) {
                super.consumeMsg(msg);

                Device.this.setData(msg);
                Device.this.panel.reRender();
            }
        }

    }
}
