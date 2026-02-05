# F4F Guidelines for ROS2 and Coding Practices

This document aims to help you standardise your work, such that your colleages can more easily navigate your code in order to revise or reuse it. This guide will help you to setup your environment, workflow, and suggest best coding practices. It'll cover most essential parts you'll need for your work: `Git` `ROS2` `C++` `MRS UAV System`.

This repository also provides a template **ROS2 Package** which you can use to quickly start your new project. It contains all the necessary components and runs a simple **TMUX** session with a basic ros component (nodelet). The source code provides an overview of how to use [MRS Lib](https://ctu-mrs.github.io/docs/features/libraries/) features that makes using `ROS2` a bit more managaeble. To use this template to create a new repository, visit [this Github page](https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-repository-from-a-template).

## Prerequisities

You should have a computer with **Ubuntu 24.04** installed. You might also need **Windows** or older **Ubuntu 20.04** for `ROS1`. If that happens, configure your PC for dual/tripple boot. You can ask for a Windows key and someone will probably have a boot device with Ubuntu.

We also have an extensive [documentation](https://ctu-mrs.github.io/docs/introduction) centered mostly around the **MRS UAV System**, but also including other usefull tutorials.

If you're feeling brave and you want a superb environment for coding workflow, ask around about Tomáš Báča's [Linux Setup](https://github.com/klaxalk/linux-setup) (branch 24.04). Or setup your `I3` or similar environment by yourself and start using `vim` for some basic code editing.


## Git setup

### Account and SSH Key

Create a [GitHub](https://github.com/) account using your company email address `name@fly4future.com` and set your `git config --global user.email "name@fly4future.com"`.

You should generate an **SSH Key** if you don't have one yet and link it to your account.
```
ssh-keygen -t ed25519 -C "name@fly4future.com"
```
When it says *"Enter file in which to save the key"* just press **Enter**. When it asks for a passphrase, you can press **Enter** twice to leave it empty or add a password for extra security. Once that's done, copy your public key. It starts with *ssh* and ends with your email.

```
cat ~/.ssh/id_ed25519.pub
```

Go to you [GitHub SSH Setting](https://github.com/settings/keys), click **New SSH Key** and paste it there. Run this to verify that **GitHub** recognizes you.
```
ssh -T git@github.com

-> Hi [Your Username]! You've successfully authenticated, but GitHub does not provide shell access.
```

### Global Gitignore

Next you can setup your global gitignore in your home directory for most common items.
```
touch .gitignore_global
git config --global core.excludefile ~/.gitignore_global
```

Open the file and paste there the following items. Feel free to add/remove any.
```
*.swp
*.swo

__pycache__
*/.tmuxinator.yml

.cache
.vscode

compile_commands.json
```

### Git Folder

We suggest to create a git folder for all your cloned repositories/packages from where you can link them to your existing workspaces.
```
$HOME/
└── git/
    │── your_git_repo_1
    │── your_git_repo_2
    └── ...
```
You can then symlink your package into your workspace source folder.
```
cd your_ws/src

ln -sf ~/git/your_git_repo_1 .
```

## ROS2 Setup
Now we'll go through a TLDR steps to configure your `ROS2` natively. You can follow the official `ROS2` tutorials or the `CTU-MRS` documentation mentioned above for more.

### Install ROS2 Jazzy
```
sudo apt-get -y install software-properties-common curl bash

curl https://ctu-mrs.github.io/ppa2-stable/add_ros_ppa.sh | bash

sudo apt-get -y install ros-jazzy-desktop-full ros-dev-tools
```

### Install MRS UAV System
```
curl https://ctu-mrs.github.io/ppa2-stable/add_ros_ppa.sh | bash

sudo apt install ros-jazzy-desktop-full

curl https://ctu-mrs.github.io/ppa2-stable/add_ppa.sh | bash

sudo apt install ros-jazzy-mrs-uav-system-full
```

Set Zenoh to be the used RMW implementation. The Zenoh RMW is used by default in our example simulation sessions. Add to `~/.bashrc` (or `~/.zshrc`):
```
export RMW_IMPLEMENTATION="rmw_zenoh_cpp"
```

Source `~/.bashrc` (or `~/.zshrc`):
```
source ~/.bashrc
```

Start the example simulation session to confirm that everything installed properly.

```
cd /opt/ros/jazzy/share/mrs_multirotor_simulator/tmux/mrs_one_drone

./start.sh
```

### Setup Your Workspace

To compensate some `colcon build` drawbacks and allow you to use `cb` command anywhere in the workspace, get the following aliases.
```
cd ~/git

git clone git@github.com:ctu-mrs/mrs_uav_development.git

cd mrs_uav_development

git checkout ros2
```
Prepare your workspace
```
mkdir -p ~/ros2_ws/src
```

Add the following to your `~/.bashrc` (or `~/.zshrc`):
```
# workspace to be sourced
export ROS_WORKSPACE="$HOME/ros2_ws"

# ROS DEVELOPMENT
# * source this after exporting $ROS_WORKSPACE="<path to your workspace>"
# * workspace is automatically sourced and the sourcing is cached
# * to force-source a workspace after adding new packages, call `presource_ros`
source $HOME/git/mrs_uav_development/shell_additions/shell_additions.sh
```

These shell additions allow for faster compilation, as it cashes parts that take long to build (mostly python parts). This cache is then being presourced, but adding new packages that require some of these components to be rebuild might require to call `presource_ros` in the workspace.

Clone an existing ROS2 package and link it to your workspace.
```
cd ~/git

git clone git@github.com:ctu-mrs/mrs_core_examples.git

cd ~/ros2_ws/src

ln -sf ~/git/mrs_core_examples/cpp/example_waypoint_flier
```

### Configure Colcon

Before building the workspace, we'll setup some flags that will be used with each `colcon build` using `mixin`.
```
sudo apt install python3-colcon-mixin

colcon mixin add default https://raw.githubusercontent.com/colcon/colcon-mixin-repository/master/index.yaml
colcon mixin add mrs https://raw.githubusercontent.com/ctu-mrs/colcon_mixin/refs/heads/master/index.yaml
colcon mixin update
```

Create a config file and copy the following build flags.
```
cd ~/ros2_ws

touch colcon_defaults.yaml
```
```
build:
  symlink-install: True     # link configs and other files to install folder
  continue-on-error: True   # build the rest of the packages if one fails
  executor: parallel        # enable parallel package building
  parallel-workers: 4       # maximum packages build in parallel (increase if good CPU)
  mixin:
    - rel-with-deb-info     # display more info when failed
    - compile-commands      # generate compile commands
```

By default, `colcon` uses a monochromatic output that is poorly readable. To add some colors and order, install the following extenstion.
```
pip install git+https://github.com/cottsay/colcon-ansi-colors-example --break-system-packages
```

Put the build option to your `~/.bashrc` (or `~/.zshrc`) to use it by default.
```
export COLCON_DEFAULT_OUTPUT_STYLE=catkin_tools
```

Open a new terminal and `source ~/.bashrc`.

### Build the Workspace

Now you can build your workspace and check, if all the configuration works.
```
cd ~/ros2_ws/

colcon init

colcon build
```

You should be able to call just `cb` out of any subdirectory of your workspace.

Since you've already put the path to your workspace to your `~/.bashrc` (or `~/.zshrc`), it should `source install/setup.bash` (or `source install/setup,ths`) automatically when you open a new terminal.

### Adding a New Workspace

In `ROS2`, if you source a new workspace, it will be automatically linked with the previous one and it will adopt its dependencies, but will not refresh them if you make any changes. That can very well compromise your new workspace and you might be forced to clean the workspace often. Therefore we encourage you to modify the path to your new workspace in `~/.bashrc` (or `~/.zshrc`) if you create a new one. The `shell_additions` will take care of the sourcing for you.

```
# workspace to be sourced
export ROS_WORKSPACE="$HOME/new_ros2_ws"
```