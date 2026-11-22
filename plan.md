# 开发计划

## 2026-11-21 模拟相机图像接通

- 新增arm专用四色圆标检测/PnP与图像适配入口，消费像素与相机内参/40mm圆心间距，不接受目标真值。
- 必须四色各唯一、正深度、重投影<=1px且ArmObservation数值合法；置信度1只代表合成检测有效，不是统计标定概率。
- 末端MuJoCo相机渲染图像已接后端坐标变换、绕行和反馈；只验证固定标记板、单次静止观测，不冒充真实物体模型。
- 验证：新增3条圆标GTest、前端CTest9/9测试程序通过；原有模型用例仍可能按缺夹具跳过。后端及仿真测试另见后端plan。

## 2026-11-14 include业务分区（覆盖旧路径）

验证：加载ROS Humble环境后重新构建，CTest 8/8 测试程序通过；仓内旧根路径include扫描为0，git diff --check通过。未运行真实硬件，外部历史调用方尚未迁移。

头文件与src同步按业务分区；装甲板armor/、机械臂arm/，前端检测抽象放common/。所有仓内源码、工具与测试include同步更新。库目标、命名空间和算法不变；这是公共包含路径破坏性变更，不保留旧路径转发头，外部调用方须同步改include。未改其他工作区或历史模拟器源码，未commit/push。


## 2026-11-14 源码业务分区

验证：迁移后重新配置/构建前端，CTest 8/8测试程序通过；git diff --check通过。测试夹具未提供的模型用例按原逻辑跳过，不代表实机验收。

按本人要求，将装甲板实现迁入src/armor，机械臂实现归src/arm，跨业务注册表归src/common。CMake同步源文件路径，公共头文件/命名空间/算法不变。前端观测校验由inline移为src/arm/observation.cpp，测试语义不变。未commit/push，PR仍到dev。

## 2026-11-14 机械臂分支

验证：完整CMake构建通过，CTest 8/8测试程序通过（模型夹具缺失的用例可能按原测试逻辑跳过，不代表真实模型/相机验收）。
- 从 origin/dev ac11181 建立 feat/arm-frontend，PR目标dev；未commit/push。
- 新增ArmObservation完整相机系旋转/平移契约和数值检查，namespace diesel_vision_frontend。
- 无相机/数据集，不实现假识别器；未改变既有Detector/Observation契约。
- interfaces正式消息与前后端装配适配待评审。

## 任务列表

- [ ] 接装甲板检测/跟踪，产出带置信度的目标
- [x] 早期单模型 Detector 已从前端移除；公共检测结果类型独立，正式入口只接双模型
- [ ] 完成双模型输入/输出边界检查、真实相机验证及与 PnP 的调用链实测（纯数据调用链测试已通过）
- [x] 增加独立海康 MVS 相机只读调试入口；仅显示双模型检测和可选相机系 PnP，不接云台控制
- [ ] 现场用当前相机、实测装甲板外轮廓尺寸与同分辨率标定核对角点和距离；记录漏检/错配样本
- [ ] 实现多源态势融合（去重 / 坐标统一 / 时间对齐 / 置信度仲裁）
- [ ] 生成 `WorldSnapshot`（对齐 `diesel_interfaces` 的冻结 schema）
- [x] 实现双检测结果的原图像素框中心配对纯函数与歧义测试（尚未接入双模型推理）
- [x] 双 ONNX 推理入口离线实模加载和同帧推理通过
- [x] 双模型共享一次预处理并并行运行独立 Session；固定图片耗时基线与结果一致性测试已加入
- [x] 正式 `process_armor_frame` 图像入口改为双模型外轮廓四角 → PnP；旧单模型与灯条适配不在真实链路中
- [x] 将双模型外轮廓四角到 SP 灯条端点的平面几何转换从模拟器移入 Vision，并给出尺寸显式输入与独立测试；仿真尺寸不可当作实车标定
- [x] 统一 Vision 输出类别为 -1/0/1/2；2 重装仅预留，未训练识别
- [x] 纠正当前 Pose 模型的红蓝数字契约（0=红、1=蓝），同步本地训练配置、PT/ONNX 名称和部署映射；新增映射回归测试
- [ ] 现场采集同一帧包含红蓝目标的相机图，核对相机 BGR、模型数字输出和最终颜色；其他 Pose 权重使用前逐个核对契约
- [ ] 复核双模型关键点标注顺序、置信度门限和颜色冲突；加入可调 HSV 仲裁，完成模拟器画面验收
- [ ] 生成 `WorldSnapshot`（对齐 `diesel_interfaces` 的冻结 schema）
- [ ] 保证 `WorldSnapshot` 生成是纯函数、零 ROS 依赖

## 当前 Agent State

PLAN_READY

## Before Snapshot

commit hash:   （首次提交）
branch:        main
modified files: （首次初始化）
risk level:    L1

## 2026-11-14 前端收敛（I3）

- [x] `git rebase dev` 重放 7 个提交（含双模型、类别契约、相机调试入口）
- [x] 命名收敛：namespace / include 路径 / include guard / 测试环境变量统一 `diesel_vision_frontend`、`DIESEL_TEST_*`
- [x] 删除旧单模型 `detector.hpp` / `detector.cpp` / `test_detector.cpp`，`DualDetector` 走 `armor_detection.hpp`
- [x] 本地 CMake 构建通过（`-DONNXRUNTIME_DIR=.dsh-tmp/onnxruntime`），`ctest` 5 组 16 条全绿
- [x] 备份分支 `backup/sp-dual-pre-rebase-1114` 保留 rebase 前状态

## 模糊点与待确认项

- 本仓接口尚未冻结，任务清单为**方向性**的，落地顺序以 `foray_docs/algorithm_structure.md` §9 演进阶段为准
- 依赖的自研仓尚未建齐，跨仓任务需等对方 `README.md` 明确接口后再启动

## Vector Backend Status

Backend: Markdown
Status:  ready
Environment: 仓库内 Markdown 文档（无外部向量后端）
Index: 本仓 `README.md` / `tree.md` / `decision.md`
Initialization: 2026-02-19
Commit: （首次提交）

## Acceptance Criteria

- [ ] 本仓能独立 `colcon build`（无代码时跳过）
- [x] 本地工作区中 `diesel_vision_frontend` 可构建且 PnP/Detector/调用链单元测试通过
- [x] 本地工作区中 `diesel_vision_frontend` 可构建且 PnP/双模型/调用链单元测试通过
- [x] 双模型固定图分段计时与 CPU ORT 线程数对照完成；下一步需模拟器逐帧验证 Tracker 和端到端耗时
- [x] 本地 `cmake` + `ctest` 在 diesel_vision_frontend 命名下 5 组 16 条全绿（ONNX Runtime 1.20.1）
- [ ] CI 绿灯
- [ ] 每个新增模块带独立可执行测试

## 2026-11-14 I4 仓名与包名迁移

- GitHub 仓 `diesel_vision` → `diesel_vision_frontend`，本地目录与 `origin` 同步；PR #2 先合入 `dev`。
- 包名 / CMake `project()` / `include/diesel_vision_frontend/` / `namespace diesel_vision_frontend` / guard `DIESEL_VISION_FRONTEND_*` / 调试可执行 `diesel_vision_frontend_camera_debug` 全量改名。
- 本地验证：`cmake -DONNXRUNTIME_DIR=... -DBUILD_TESTING=ON` 构建通过，5 组 16 条测试全绿。
- 下一步：I5 前端多目标抽象（TargetClass / Observation / Detector / registry）。

## 2026-11-14 I5 前端多目标抽象

- 新增 `TargetClass` / `Observation` / `Detector` / `DetectorRegistry` / `DetectorConfig`，装配走 `config/vision.conf`。
- `ArmorDetector` 成为首个 `TargetClass::Armor` 实现；无标定时只输出像素四角，`normal_valid=false`。
- 验收：`test/test_detector_registry.cpp` 用假检测器证明「1 个实现 + 1 行注册 + 1 段配置」即可接入，并覆盖未注册名/重复注册/重复键/非数字参数的报错。
- 本地：前端 7 个测试程序 28 条全绿（含 registry 11 条、armor_detector 3 条其中 2 条按夹具跳过）；与后端 83 条合计 111 条。
