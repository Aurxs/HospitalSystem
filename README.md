# 医院管理系统 (Hospital Management System)

## 一、 编译与运行指南

本项目支持 Windows、macOS 和 Linux 平台。推荐使用 CMake 进行构建，也可以直接使用 GCC 编译。

### 1. 环境准备

在编译之前，请确保已安装 C 编译器（GCC 或 Clang）以及 `ncurses` 图形库。

* **Linux (Ubuntu/Debian)**:

  ```bash
  sudo apt-get install build-essential libncurses5-dev libncursesw5-dev cmake
  ```

* **macOS**:
  macOS 通常预装了 ncurses 库，建议安装 Xcode Command Line Tools 和 CMake。

  ```bash
  xcode-select --install
  brew install cmake
  ```

* **Windows (详细步骤)**:
  Windows 系统默认没有 curses 库，强烈推荐使用 `vcpkg` 包管理器安装 `pdcurses`。

    1. **安装 vcpkg**:
       打开 PowerShell 或 CMD，执行以下命令下载并配置 vcpkg：

       ```cmd
       git clone https://github.com/microsoft/vcpkg.git
       cd vcpkg
       .\bootstrap-vcpkg.bat
       ```

    2. **安装依赖包**:

       ```cmd
       .\vcpkg install pdcurses:x64-windows
       ```

    3. **编译运行**:
       回到本项目目录，创建 build 文件夹并配置 CMake（注意替换 `[path/to/vcpkg]` 为实际 vcpkg 的安装路径）：

       ```cmd
       mkdir build
       cd build
       cmake .. "-DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake"
       cmake --build .
       ```

       编译完成后，在 Debug 目录下运行 `HospitalSystem.exe`。

### 2. 使用 CMake 编译 (推荐)

CMake 是一个跨平台的构建工具，能够自动处理依赖关系和编译选项。

**步骤：**

1. 在项目根目录下创建一个构建文件夹：

   ```bash
   mkdir build
   cd build
   ```

2. 生成 Makefile 或项目文件：

   ```bash
   cmake ..
   ```

3. 编译项目：

   ```bash
   cmake --build .
   ```

4. 运行生成的程序：

    * Linux/macOS: `./HospitalSystem`
    * Windows: `HospitalSystem.exe`

### 3. 使用 GCC 直接编译

#### Linux / macOS (ncurses)

Linux 或 macOS 用户通常可以直接使用系统自带的 ncurses 库。

**命令：**

```bash
gcc -o hospital main.c src/*.c -Iinclude -lncurses
```

*(注：如果遇到宽字符乱码问题，尝试链接 `ncursesw`：`-lncursesw`)*

#### Windows (MinGW + PDCurses)

Windows 用户使用 MinGW GCC 编译时，需要手动配置 PDCurses 库。

1. **准备环境**:

    * 确保已安装 MinGW-w64。
    * 下载 [PDCurses](https://github.com/wmcbrine/PDCurses) 源码并编译，或者下载预编译好的 `pdcurses.a` 和 `curses.h`。

2. **放置文件**:

    * 将 `curses.h` 放到项目的 `include` 文件夹中（或者单独放在一个文件夹并在编译时通过 `-I` 指定）。
    * 将 `pdcurses.a` 放到项目根目录（或新建 `lib` 目录）。

3. **编译命令**:
   打开终端（CMD/PowerShell），在项目根目录下执行（假设 `pdcurses.a` 在当前目录）：

   ```cmd
   gcc -o hospital.exe main.c src/*.c -Iinclude -L. -lpdcurses
   ```

**通用参数说明：**

* `-o hospital`: 指定输出文件名。
* `main.c src/*.c`: 编译所有源文件。
* `-Iinclude`: 指定头文件搜索路径。
* `-L.`: (Windows) 指定库文件搜索路径为当前目录。
* `-lpdcurses`: (Windows) 链接库名为 `pdcurses` 的库文件。
* `-lncurses`: (Linux/Mac) 链接 ncurses 库。

**运行：**

```bash
./hospital      # Linux/macOS
.\hospital.exe  # Windows
```

### 4. 测试账号与初始密码（基于 `data/auth.txt`）

为方便他人下载后直接测试，仓库内示例数据已预置以下登录账号：

| 角色  | 用户名                            | 初始密码       |
|-----|--------------------------------|------------|
| 管理员 | `admin`                        | `admin123` |
| 医生  | `data/doctors.txt` 第一列中的全部用户名  | `123456`   |
| 患者  | `data/patients.txt` 第一列中的全部用户名 | `123456`   |

> 说明：`data/auth.txt` 中密码为哈希存储，表中的密码是“登录时输入的明文”。

当前示例医生用户名（与 `data/doctors.txt` 一致）：
`王医生`、`赵医生`、`李医生`、`孙医生`、`周医生`、`吴医生`、`郑医生`、`冯医生`、`陈医生`、`蒋医生`、`韩医生`、`林医生`、`贾医生`、
`邹医生`、`许医生`、`何医生`、`吕医生`、`施医生`、`董医生`、`田医生`。

当前示例患者用户名（与 `data/patients.txt` 一致）：
`张三`、`李四`、`王五`、`赵六`、`钱七`、`孙八`、`周九`、`吴十`、`郑十一`、`冯十二`、`陈十三`。

---

## 二、 项目总体设计

本系统是一个基于 C 语言开发的医院管理系统，旨在模拟医院的日常业务流程。系统采用模块化设计思想，将不同的业务逻辑分离，便于维护和扩展。用户界面（UI）部分使用了
`ncurses` 库，在终端中实现了图形化的交互体验。

### 核心模块组成

系统主要由以下几个核心模块组成：

1. **用户权限管理模块 (Auth)**
    * **功能**: 负责系统的登录验证、用户账号管理（增删改查）以及权限控制。
    * **角色**: 系统支持三种角色：管理员（Admin）、医生（Doctor）、患者（Patient）。

2. **患者管理模块 (Patient)**
    * **功能**: 维护患者的个人档案，包括基本信息（姓名、年龄、性别、联系方式）以及医疗记录（诊断结果、治疗方案）。

3. **医生管理模块 (Doctor)**
    * **功能**: 维护医生的个人档案，包括基本信息、所属科室、排班信息等。

4. **药品管理模块 (Drug)**
    * **功能**: 管理医院的药品库，记录药品的名称、规格、厂家、价格及库存数量。

5. **挂号管理模块 (Registration)**
    * **功能**: 处理患者的挂号请求，建立患者与医生/科室之间的就诊关系，记录挂号日期。

6. **费用管理模块 (Bill)**
    * **功能**: 记录患者在就诊过程中产生的各项费用（如药品费、检查费），并提供费用统计功能。

7. **数据持久化模块 (File IO)**
    * **功能**: 负责将内存中的链表数据保存到文本文件中，以及在系统启动时从文件加载数据，保证数据不丢失。

8. **用户界面模块 (UI)**
    * **功能**: 封装了所有的界面显示逻辑，提供菜单导航、表单输入、数据展示等交互功能。

---

## 三、 详细设计流程

本部分将详细介绍每个模块的功能设计、核心函数及其实现方法。

### 1. 用户权限管理模块 (Auth)

该模块主要用于管理登录系统的用户信息。

* **核心函数**:
    * `AuthNode make_user(const char *username, const char *password, int role)`
        * **功能**: 创建一个临时的用户节点对象。
        * **实现**: 接收用户名、密码和角色，将其填充到 `AuthNode` 结构体中并返回。
    * `AuthNode *add_user(AuthNode **head, AuthNode newInfo)`
        * **功能**: 向用户链表中添加新用户。
        * **实现**: 动态申请内存创建新节点，将 `newInfo` 的数据拷贝进去，采用头插法或尾插法将其加入链表。
    * `AuthNode *find_user(AuthNode *head, const char *username)`
        * **功能**: 查找指定用户名的用户。
        * **实现**: 遍历链表，使用 `strcmp` 比对用户名，匹配成功则返回节点指针，否则返回 `NULL`。
    * `AuthNode *modify_user(AuthNode *head, const char *username, AuthNode newInfo)`
        * **功能**: 修改用户信息。
        * **实现**: 先调用 `find_user` 找到节点，然后更新其密码或角色信息。
    * `AuthNode *delete_user(AuthNode *head, const char *username)`
        * **功能**: 删除指定用户。
        * **实现**: 遍历链表找到目标节点的前驱节点，修改指针指向以移除目标节点，并释放其内存。
  * `int authenticate_user(AuthNode *head, const char *username, const char *password)`
      * **功能**: 验证用户名和密码。
      * **实现**: 遍历链表匹配用户名，验证密码（通常涉及解密比对）。

### 2. 患者管理模块 (Patient)

该模块用于维护患者的详细信息。

* **核心函数**:
    * `PatientNode make_patient(...)`
        * **功能**: 构造患者信息节点，包含姓名、年龄、性别、电话、诊断、治疗方案。
    * `PatientNode *add_patient(PatientNode **head, PatientNode newInfo)`
        * **功能**: 注册新患者。
        * **实现**: 在链表中分配新节点并存储患者信息。
    * `PatientNode *findPatient_name(PatientNode *head, const char *name)`
        * **功能**: 按姓名查找患者。
        * **实现**: 遍历链表，比对 `name` 字段。
    * `PatientNode *findPatient_phone(PatientNode *head, const char *phone)`
        * **功能**: 按电话查找患者（通常电话更具唯一性）。
        * **实现**: 遍历链表，比对 `phone` 字段。
    * `PatientNode *modify_patient(...)`
        * **功能**: 更新患者信息（如更新诊断结果）。
        * **实现**: 查找对应患者节点并覆盖旧数据。
  * `PatientNode *delete_patient(PatientNode *head, const char *phone)`
      * **功能**: 删除患者信息。
      * **实现**: 根据电话号码找到节点并从链表中移除。
  * `PatientNode *sort_patients_by_name(PatientNode *head)`
      * **功能**: 按姓名字典序排序患者链表。
  * `PatientNode *sort_patients_by_phone(PatientNode *head)`
      * **功能**: 按电话号码字典序排序患者链表。
  * `PatientNode *sort_patients_by_age(PatientNode *head)`
      * **功能**: 按年龄升序排序患者链表。

### 3. 医生管理模块 (Doctor)

该模块用于医院人力资源管理，维护医生信息。

* **核心函数**:
    * `DoctorNode make_doctor(...)`
        * **功能**: 构造医生信息节点，包含姓名、科室、排班等。
    * `DoctorNode *add_doctor(DoctorNode **head, DoctorNode newInfo)`
        * **功能**: 录入新医生。
    * `DoctorNode *findDoctor_name(...)` / `findDoctor_phone(...)`
        * **功能**: 查找医生信息。
    * `DoctorNode *modify_doctor(...)`
        * **功能**: 修改医生信息（如调整排班、晋升职称等）。
  * `DoctorNode *delete_doctor(DoctorNode *head, const char *phone)`
      * **功能**: 删除医生信息。
  * `DoctorNode *sort_doctors_by_name(DoctorNode *head)`
      * **功能**: 按姓名字典序排序医生链表。
  * `DoctorNode *sort_doctors_by_phone(DoctorNode *head)`
      * **功能**: 按电话号码字典序排序医生链表。
  * `DoctorNode *sort_doctors_by_age(DoctorNode *head)`
      * **功能**: 按年龄升序排序医生链表。

### 4. 药品管理模块 (Drug)

该模块用于药房管理。

* **核心函数**:
    * `DrugNode make_drug(...)`
        * **功能**: 构造药品节点，包含名称、规格、厂家、价格、库存。
    * `DrugNode *add_drug(...)`
        * **功能**: 药品入库。
    * `DrugNode *findDrug_name(...)`
        * **功能**: 查询药品信息。
    * `DrugNode *modify_drug(...)`
        * **功能**: 修改药品信息（如调整价格、更新库存）。
    * `DrugNode *delete_drug(...)`
        * **功能**: 药品下架/删除。
  * `DrugNode *sort_drugs_by_price(DrugNode *head)`
      * **功能**: 按价格升序排序药品链表。
  * `DrugNode *sort_drugs_by_stock(DrugNode *head)`
      * **功能**: 按库存数量升序排序药品链表。

### 5. 挂号管理模块 (Registration)

该模块连接患者与医生，是就诊流程的开始。

* **核心函数**:
    * `RegisterNode make_registration(...)`
        * **功能**: 构造挂号单，记录患者名、医生名、科室、日期。
    * `RegisterNode *add_registration(...)`
        * **功能**: 提交挂号记录。
    * `RegisterNode *findRegistration_patient(...)`
        * **功能**: 查询某位患者的挂号记录。
    * `RegisterNode *findRegistration_doctor(...)`
        * **功能**: 查询某位医生的待诊列表。
  * `RegisterNode *findRegistration_department(RegisterNode *head, const char *department)`
      * **功能**: 通过科室查找挂号记录。
  * `RegisterNode *findRegistration_date(RegisterNode *head, const char *date)`
      * **功能**: 通过日期查找挂号记录。
  * `RegisterNode *delete_registration(RegisterNode *head, const char *patientName, const char *date)`
      * **功能**: 删除挂号记录。

### 6. 费用管理模块 (Bill)

该模块处理财务相关业务。

* **核心函数**:
    * `BillNode make_bill(...)`
        * **功能**: 生成费用单据，记录项目名和金额。
    * `BillNode *add_bill(...)`
        * **功能**: 增加一笔费用记录。
    * `double calculate_total_bill(BillNode *head, const char *patientName)`
        * **功能**: 结算。
        * **实现**: 遍历费用链表，筛选出该患者的所有费用记录，累加 `amount` 字段并返回总额。
  * `BillNode *findBill_patient(BillNode *head, const char *patientName)`
      * **功能**: 通过患者姓名查找费用记录。
  * `BillNode *findBill_item(BillNode *head, const char *itemName)`
      * **功能**: 通过收费项目查找费用记录。
  * `BillNode *modify_bill(BillNode *head, const char *patientName, const char *itemName, BillNode newInfo)`
      * **功能**: 修改费用记录。
  * `BillNode *delete_bill(BillNode *head, const char *patientName, const char *itemName)`
      * **功能**: 删除费用记录。
  * `BillNode *sort_bills_by_amount(BillNode *head)`
      * **功能**: 按金额升序排序费用链表。

### 7. 数据持久化模块 (File IO)

该模块负责数据的存取，确保系统重启后数据依然存在。

* **核心函数**:
    * `save_patients` / `save_doctors` / `save_drugs` ...
        * **功能**: 将各模块的链表数据写入对应的文本文件（如 `patients.txt`）。
        * **实现**: 使用 `fopen` 打开文件（写模式），遍历链表，使用 `fprintf` 将每个节点的字段按特定格式写入文件，最后
          `fclose` 关闭文件。
    * `load_patients` / `load_doctors` / `load_drugs` ...
        * **功能**: 从文件中读取数据重建链表。
        * **实现**: 使用 `fopen` 打开文件（读模式），循环使用 `fscanf` 读取数据，调用 `make_xxx` 和 `add_xxx`
          函数将读取的数据构建成链表，直到文件结束。

### 8. 用户界面模块 (UI)

该模块负责处理所有与用户的交互，使用 `ncurses` 库构建终端图形界面。

* **核心控制与通用组件**:
    * `int ui_init(void)`
        * **功能**: 初始化 ncurses 环境，配置颜色对、键盘模式等。
    * `void ui_cleanup(void)`
        * **功能**: 退出 ncurses 模式，恢复终端设置。
    * `int ui_main(void)`
        * **功能**: UI 模块入口，负责调度登录界面和主界面。
    * `int ui_login_screen(void)`
        * **功能**: 显示登录框，调用 Auth 模块验证用户身份。
    * `void ui_main_screen(int role)`
        * **功能**: 根据登录用户的角色（管理员/医生/患者）显示不同的主菜单和侧边栏。
    * `void ui_show_message(const char *title, const char *message, int type)`
        * **功能**: 显示通用消息弹窗（成功/错误/警告）。
    * `int ui_confirm_dialog(const char *title, const char *message)`
        * **功能**: 显示 Yes/No 确认对话框。
    * `int ui_input_string(...)` / `ui_input_int(...)` / `ui_input_double(...)`
        * **功能**: 封装的输入函数，支持在指定窗口位置获取各种类型的输入。

* **业务管理界面 (通用/管理员)**:
    * `void ui_patient_management(WINDOW *win)`
        * **功能**: 患者信息的列表展示、分页、选中交互。
    * `void ui_doctor_management(WINDOW *win)`
        * **功能**: 医生信息管理界面。
    * `void ui_drug_management(WINDOW *win)`
        * **功能**: 药品库存管理界面。
    * `void ui_registration_management(WINDOW *win)`
        * **功能**: 挂号记录管理界面。
    * `void ui_bill_management(WINDOW *win)`
        * **功能**: 费用记录管理界面。
    * `void ui_user_management(WINDOW *win)`
        * **功能**: 系统用户账号管理界面（仅管理员）。
    * **配套表单与菜单**: 包含各模块的添加表单 (`ui_add_xxx_form`)、修改表单 (`ui_modify_xxx_form`)、查询菜单 (
      `ui_search_xxx`) 和排序菜单 (`ui_sort_xxx_menu`)。

* **特定角色功能**:
    * **患者端**:
        * `ui_patient_main_screen()`: 患者专属主屏幕。
        * `ui_patient_register()`: 患者自助挂号界面。
        * `ui_patient_view_registrations()`: 查看个人挂号记录。
        * `ui_patient_view_bills()`: 查看个人费用详情。
    * **医生端**:
        * `ui_doctor_main_screen()`: 医生专属主屏幕。
        * `ui_doctor_patient_management()`: 管理挂号到该医生的患者。
        * `ui_doctor_registration_management()`: 查看待诊列表。
        * `ui_doctor_bill_management()`: 为患者开具费用单。

---

## 四、 公有数据结构

系统使用链表作为主要的数据结构来存储各类信息。所有数据结构定义在 `include/datastruct.h` 中。

### 1. 患者节点 (PatientNode)

用于存储患者的个人及医疗信息。

```c
typedef struct PatientNode {
    char name[MAX_NAME];         // 姓名
    int age;                     // 年龄
    char gender[MAX_GENDER];     // 性别
    char phone[MAX_PHONE];       // 联系方式（唯一标识建议）
    char diagnosis[MAX_DESC];    // 诊断结果
    char treatment[MAX_DESC];    // 治疗方案
    struct PatientNode* next;    // 指向下一个患者节点的指针
} PatientNode;
```

### 2. 医生节点 (DoctorNode)

用于存储医生的职业信息。

```c
typedef struct DoctorNode {
    char name[MAX_NAME];         // 姓名
    int age;                     // 年龄
    char gender[MAX_GENDER];     // 性别
    char department[MAX_DEPT];   // 所属科室
    char phone[MAX_PHONE];       // 联系电话
    char schedule[MAX_DESC];     // 排班信息
    struct DoctorNode* next;     // 指向下一个医生节点的指针
} DoctorNode;
```

### 3. 药品节点 (DrugNode)

用于存储药品的库存和属性信息。

```c
typedef struct DrugNode {
    char name[MAX_NAME];         // 药品名称
    char spec[MAX_DEPT];         // 规格 (如: 10mg/片)
    char factory[MAX_NAME];      // 生产厂家
    double price;                // 单价
    int stock;                   // 库存数量
    struct DrugNode* next;       // 指向下一个药品节点的指针
} DrugNode;
```

### 4. 挂号节点 (RegisterNode)

用于关联患者和医生，表示一次就诊预约。

```c
typedef struct RegisterNode {
    char patientName[MAX_NAME];  // 患者姓名
    char doctorName[MAX_NAME];   // 医生姓名
    char department[MAX_DEPT];   // 挂号科室
    char date[MAX_NAME];         // 就诊日期
    struct RegisterNode* next;   // 指向下一个挂号节点的指针
} RegisterNode;
```

### 5. 费用节点 (BillNode)

用于记录单笔消费详情。

```c
typedef struct BillNode {
    char patientName[MAX_NAME];  // 患者姓名
    char itemName[MAX_NAME];     // 收费项目名称
    double amount;               // 金额
    struct BillNode* next;       // 指向下一个费用节点的指针
} BillNode;
```

### 6. 用户节点 (AuthNode)

用于系统登录验证。

```c
typedef struct AuthNode {
    char username[MAX_NAME];    // 用户名
    char password[MAX_NAME];    // 密码
    int role;                   // 角色 (0:管理员, 1:医生, 2:患者)
    struct AuthNode* next;      // 指向下一个用户节点的指针
} AuthNode;
```
