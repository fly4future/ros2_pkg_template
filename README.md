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

## Coding Guidelines

### Code Formatting

We use `clang` with autoformatting and code-completion to help us normalize our codes. This improves overall code readability not only for you, but for other reading your code. The `.clang-format` file with our formatting looks like his:
```
Language: Cpp
Standard: c++20

AccessModifierOffset: -2
ColumnLimit: 160
MaxEmptyLinesToKeep: 2

AlignAfterOpenBracket: Align
AlignConsecutiveAssignments: true
AlignConsecutiveDeclarations: true

AllowShortBlocksOnASingleLine: false
AllowShortCaseLabelsOnASingleLine: false
AllowShortFunctionsOnASingleLine: false
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false

AlwaysBreakTemplateDeclarations: true
BreakBeforeBinaryOperators: false
BreakBeforeBraces: Custom
BreakConstructorInitializers: BeforeColon

KeepEmptyLinesAtTheStartOfBlocks: true
NamespaceIndentation: None
SortIncludes: false
SpaceBeforeParens: ControlStatements

BraceWrapping:
  AfterClass: false
  AfterControlStatement: false
  AfterEnum: true
  AfterFunction: false
  AfterNamespace: true
  AfterObjCDeclaration: true
  AfterStruct: true
  AfterUnion: true
  BeforeCatch: true
  BeforeElse: false
  IndentBraces: false
```

### Code Formatting in VSCode

To use the autoformatting in VSCode, follow these steps:
1. Install VSCode
2. Install VSCode extensions: `C/C++`, `C/C++ Extension Pack`, `clangd`, `Run on Save` (VSCode might prompt you to disable intelliSenseEngine, do it)
3. Set up `clangd` config for C/C++ code formatting: create file `$HOME/.clang-format` in your gome directory and paste the above content inside
4. Set up `clangd` as the default formatter: `File -> Preferences -> Settings` and set `Editor: Default Formatter` to `clangd`.
5. Set up code formatting on save: `File -> Preferences -> Settings` and check `Editor: Format on Save`.

Now when you save a file (Ctrl + s), you code should automatically reformat. However, `clangd` requires file `compile_commands.json` generated by cmake run with `CMAKE_EXPORT_COMPILE_COMMANDS=ON`. In our setup, this is done automatically when `colcon build` is run, but one `compile_commands.json` is generated for each ROS package in `workspace/build/_package_/compile_commands.json`. The following setup process links the `workspace/build/_package_/compile_commands.json` into the relevant ROS package whenever the package is built. The package-specific `compile_commands.json` is then loaded by clangd.
1. In the **ROS2 package**, open `.gitignore` and add the following line, if you don't have it in your `.global_gitignore`

```
compile_commands.json
```

2. In the **ROS2 package**, open `CMakeLists.txt` and add the following lines right after the `project(you_project_name)` (this will create and link a `compile_commands.json` file in the package source dir after every `colcon build`)

```
# --- Export & Symlink compile_commands.json ---
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCE_JSON "${CMAKE_SOURCE_DIR}/compile_commands.json")
set(BINARY_JSON "${CMAKE_BINARY_DIR}/compile_commands.json")

if(EXISTS ${BINARY_JSON} AND NOT EXISTS ${SOURCE_JSON})
    message(STATUS "Symlinking compile_commands.json to source directory")
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${BINARY_JSON} ${SOURCE_JSON})
endif()
```

3. Open the **ROS2 package** in VSCode.
4. Code formatting shall work for you now. You may verify that by changing `ColumnLimit: 50` in `$HOME/.clang-format`, saving a file, and checking that each row has max. 50 characters. Revert this change back to `ColumnLimit: 160` afterwards.

We recommend opening a **ROS2 package** not from the workspace symlink, but from the source of your `~/git` folder.

### Naming Conventions

In our `C++` and `ROS2` codes, we stick to these naming rules:
- **Classes and Structures:** `YourNode` `YourStruct`
- **Variables:** `your_variable`
  - **Member Variables:** `your_variable_`
  - **Mutexes:** `mutex_your_variable_`
  - **ROS Parameters:** `_your_parameter_`
  - **ROS Interfaces:** `sub_your_topic_` `pub_your_topic_` `server_call_` `client_call_` `timer_publish_reference_`
  - **ROS Callback Groups:** `cbkgrp_subs_` `cbkgrp_clinets_` `cbkgrp_servers_` `cbkgrp_timers_`
- **Functions:** `initialize` `getDistance` `radiansToDegrees`
  - **ROS Callbacks:** `callbackYourTopic` `timerPublishReference` `callbackYourServerCall`
- **Constants and Defines:** `YOUR_CONSTANT` `YOUR_DEFINE`
- **ROS Messages and Services:** `YourMessage` `YourServer`
- **ROS Remappings:**
  - **Subscribers and Servers:** `topic_in` `service_in`
  - **Publishers and Clinets:** `topic_out` `service_out`
- **Files, Folders, Packages, Repositories:** `your_file` `your_folder` `your_package` `your_repo`
- **Git Branches:** `your-branch` `fix/your-branch` `feature/your-branch`

#### Some key points:
- All the names should be descriptive, such that you can deduce their purpose, but not overly long.

- All the names must be in english. Avoid using uncommon forms or phrases only you understand.

- Stick to infinitive and basic forms (`initialize`). Use past or continuous where appropriate (`is_initialized`, `startInitializing`).

- Always use fixed prefixes (`mutex`, `sub`, `pub`, `server`, `client`, `timer`, `cbkgrp`, `callback`).

- Always use `_in` and `_out` suffixes in your topic and service remappings in your code. It happens often that you will have to change the name of the topic/service (i.e. what you write if you `echo` or `call`). If that happens, the only thing you need to change is what it is remaped to in the launch file (no rebuilding needed).

### Best Practices

You can find (and should read) general good coding practices in our [documentation](https://ctu-mrs.github.io/docs/software/cpp/cpp_good_practices). We will mention only several important points:

- Nodelet (or component?) everything! Nodelets compared to nodes do not need to send whole messages. Multiple nodelets running under the same nodelet manager form one process, where messages can be passed as pointers.

- Do not use raw pointers! Smart pointers from `<memory>` free resources automatically, thus preventing memory leaks.

- Lock access to member variables! Nodelets are multi-thread processes, so it is our responsibility to make our code thread-safe.
  - Use `C++17` `scoped_lock` which unlocks the mutex after leaving the scope. This way, you can't forget to unlock the mutex.

- When a component (nodelet) is initialized, the method `intialize()` is called. In this method, the subscribers are initialized, and callbacks are bound to them. The callbacks can run even before the `intialize()` method ends, which can lead to some variables being still not initialized, parameters not loaded, etc. This can be prevented by using an `is_initialized_`, initializing it to `false` at the beginning of `intialize()` and setting it to `true` at the end. Every callback should check this variable and continue only when it is `true`.

- Use `mrs_lib::ParamLoader` class to load parameters from launch files and config files. This class checks whether the parameter was actually loaded, which can save a lot of debugging. Furthermore, loading matrices into config files becomes much simpler.

- For printing debug info to terminal use `RCLCPP_INFO()`, `RCLCPP_WARN()`, `RCLCPP_ERROR()` macros. Do not spam the terminal by printing a variable every time a callback is called, use for example `RCLCPP_INFO_THROTTLE(node_->get_logger(), *clock_, 1000, "dog")` to print "dog" not more often than every second.

- If you need to execute a piece of code periodically, do not use sleep in a loop, or anything similar. Instead, use `mrs_lib::TheadTimer` (or native but greedy `mrs_lib::ROSTimer`) class for this purposes, which executes a callback every time the timer expires.

- Always check whether all subscribed messages are coming. If not, print a warning. Then you know the problem is not in your nodelet and you know to look for the problem in topic remapping or the node publishing it.

- Use config parameters! Before you create a constant, think first whether you might chace it later at some point. There is a high change you will. For example, you may find out during field experiments that some delays or timer rates might need changing. And you (nor someone else) don't want to go through your code when "SSHed" to the drone, with freezing cold outside. Just put it inside the `config.yaml` file and load it as a parameter.

- Do not push unbuild or untested code to master branches on Git (or main devel branches)! Doing so can ruin experiments and drones at best!

- If you cannot figure something out, ask in **Software** Google Spaces for help. If you figure something out that did not work before, note somewhere how you solved it and you can share it in chat. There is a high chance that you (or someone else) will have to do the same thing agian.