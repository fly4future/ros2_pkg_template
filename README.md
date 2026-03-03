<!--
MARKDOWN IMAGES & BADGES
* https://www.markdownguide.org/basic-syntax/#reference-style-links
* https://github.com/Ileriayo/markdown-badges

EMOJIS
* https://gist.github.com/rxaviers/7360908
-->

<div align="center" id="readme-top">

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://github.com/user-attachments/assets/610ea197-7de0-46fc-a1a2-2c91cfe48397">
  <source media="(prefers-color-scheme: light)" srcset="https://github.com/user-attachments/assets/ece6ad5f-e44d-4196-805d-9daa7fa6fe29">
  <img alt="F4F Logo" src="https://github.com/user-attachments/assets/610ea197-7de0-46fc-a1a2-2c91cfe48397" width="80"/>
</picture>

# README title

An awesome README description!

[**Explore the docs »**](https://fly4future.github.io/ros2_pkg_template/)

</div>

## :pushpin:About The Project

![Product Name Screen Shot](https://user-images.githubusercontent.com/30635659/169600958-763d1628-b7bb-475c-a8bc-56120d1d3ff9.svg)

Here's a blank template to get started: To avoid retyping too much info. Do a _search and replace_ with your text editor `ros2_pkg_template`

## :checkered_flag:Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running, follow these simple example steps.

### Installation

#### Local

1. Clone and move it into your `ROS2` workspace

   ```sh
   cd ~/git # or the folder where you want to keep your git repositories (e.g., `~/Develop`)
   git clone git@github.com:fly4future/ros2_pkg_template.git

   cd ~/ros2_ws/src
   ln -sf ~/git/ros2_pkg_template .
   ```

   > [!NOTE]
   > Don't forget to have your SSH key added to your GitHub account. If you don't have one, you can follow this [guide](https://wiki.fly4future.com/docs/prerequisities/git-setup).

2. Next steps

#### Docker

1. Build the image
   ```sh
   docker build -t ros2_pkg_template:latest .
   ```
2. Run the container
   ```sh
   docker run -it --rm ros2_pkg_template:latest
   ```

## :balloon:Usage

You can use this space to show useful examples of how a project can be used. Additional screenshots, code examples, and demos work well in this space. You may also link to more resources.

> [!IMPORTANT]
> For more examples, please refer to the [Documentation](https://fly4future.github.io/ros2_pkg_template/).

## About the Template

## (!REMOVE THIS SECTION WHEN YOU START YOUR OWN PACKAGE)

This package is primarily intended to be used as a helper template to start your new `ROS2` package. On the [GitHub](https://github.com/fly4future/ros2_pkg_template) page, you can choose the option to **Use this template** -> **Create a new repository** to create a brand new repository with fresh history, but containing everything in this package. You will have all the basic structure that a proper **Fly4Future** `ROS2` package should have. You are free to modify whatever you want. This serves you just as a kit to get you started more quickly.

### What you need to replace/delete in this template:

- `README.md` (this file)
  - Replace `ros2_pkg_template` with the name of the repository you created.
  - Replace the title, description, and other sections with your own content.
  - Replace the image with your own logo or screenshot. You can also remove it if you don't need it.
  - If you have a different documentation page than the one provided by GitHub Pages (https://fly4future.github.io/...), replace the links to the documentation with your own.
- `package.xml`
  - Replace the name, description, maintainer, and other fields with your own content. You can also add/remove dependencies as needed.
- `CMakeLists.txt`
  - Replace the project name and other fields with your own content. You can also add/remove dependencies and targets as needed.
- `src`, `include`, `config` and `launch` folders
  - Delete the existing files and replace them with your own code, config, and launch files. You can also keep some of the files if you find them useful for your package.
- `tmux/` folder
  - Replace the `TMUX_SESSION_NAME` in `start.sh` and `kill.sh` with the name of your package or something relevant to your project. You also need to replace the `name` and `socket_name` fields in `session.yaml` with the same name.
  - Delete the existing panes and replace them with your own commands to launch your package and other tools you might need.

### How to run this Package

Then run the included `TMUX` session:

```sh
cd ros2_pkg_template/tmux
./start.sh
```

You should see a tmux session with two windows: `router` (a single pane) and `ros2_pkg_template` (split into three panes). The **top-left** window is running the main node, telling you that a permit to start publishing is `false`. If you jump to the window below (using either `tmux` binding <kbd>Ctrl</kbd>+<kbd>a</kbd>/<kbd>Ctrl</kbd>+<kbd>b</kbd> or from Linux setup <kbd>Ctrl</kbd>+<kbd>h</kbd>,<kbd>j</kbd>,<kbd>k</kbd>,<kbd>l</kbd>) and **pull** the command from history with <kbd>^</kbd>, you can allow publishing using a trigger service call. The top-right window is echoing the publishing topic, and you should see a message being received. To stop the publishing, use the service call prepared on the last remaining window.

To kill the session press <kbd>Ctrl</kbd> + <kbd>a</kbd> then <kbd>k</kbd> for kill, then <kbd>9</kbd> for this session.

### What's inside this Package

We highly encourage you to go through the files to see how we use `ROS2` with `MRS UAV System` and other useful features.

- `ros2_template.cpp`

  This file contains the nodelet with all the basic methods you might need. If you create your own nodelet, you should follow the same structure and use the `mrs_lib` wrappers to make your life easier.

  Notice the `initialize()` method where everything is set up. Take a look at how to use `param_loader` to load parameters from a config file, how to define **subscriber**, **publishers**, **timers**, **servers**, and **clients** using the **MRS library handlers**. Other features to look for:
  - **subscriber** callback and `getMsg()`
  - **timers** with and without **autostart**
  - **callback group** types and where to use **mutex**
  - **service** callback with **request/response**
  - **RCLCPP_INFO** and other printing functions

- **Launch files**

  `Python` launch files offer a bit more options than `XML`, but they are not very user-friendly. It is often sufficient to use a simpler `XML` structure that is easier to edit during the experiments. You can compare both options inside this package.

- **TMUX session**

  Launching multiple nodelets with different configs and settings is often necessary, and `tmux` sessions are a great tool for that. Take a look inside the **tmux** folder and `session.yaml` how you can launch `ROS2` nodelets, export parameters, and prepare commands. You can do much more than that, like launching `RViz` and different tools, etc.

  > [!TIP]
  > Visit the [tmuxinator](https://github.com/tmuxinator/tmuxinator) tool to create more complex sessions with a simple YAML file or [Waypoint Flier example pkg](https://github.com/ctu-mrs/mrs_core_examples/tree/ros2/cpp/example_waypoint_flier) from **MRS** to see more details.
