# 春日团长的异世界任务板 —— 软件工程设计文档

> 对应需求文档：`docs/features/03-quest-board.md`
> 版本：v1.0
> 日期：2026-05-31

---

## 1. 需求概述

### 1.1 功能目标
为 SOS 团招新页面增加一套 RPG 风格的任务系统。用户登录后可接取每日委托、周常挑战与传说任务，完成后获得 SOS 团贡献值（points）。任务板是提升用户留存与页面互动深度的核心扩展功能。

### 1.2 用户确认的需求约束
| 约束项 | 确认结果 |
|--------|----------|
| 数据持久化 | **需要后端支持**，不能仅依赖 localStorage |
| 鹤屋商店 | 本次仅**预留接口与前端占位**，不实现兑换逻辑 |
| 春日脑洞任务 | **趣味性优先**，难度要低，避免用户挫败感 |
| 任务分享 | **需要**生成任务完成截图 |

### 1.3 功能范围（Scope）
**In Scope（本次实现）**
- 每日委托（4 个/日，0 点刷新）
- 周常挑战（每周一刷新）
- 传说任务（一次性，条件触发解锁）
- 春日脑洞随机任务（4 个中的 1 个随机位）
- 任务进度追踪与状态流转
- 贡献值账户（增/查）
- 任务完成动画与分享截图
- 鹤屋商店前端占位页（仅展示商品列表，按钮 disabled）

**Out of Scope（后续迭代）**
- 鹤屋商店兑换逻辑与库存系统
- 多用户排行榜/社交对比
- 任务系统的管理后台（增删改任务配置）
- 真实支付/广告解锁补签卡

---

## 2. 系统架构

### 2.1 整体架构图

```
┌──────────────────────────────────────────────────────────────┐
│                        前端 (Vue3)                            │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │ QuestBoard  │  │ QuestCard   │  │ QuestShareModal     │  │
│  │   主页面    │  │   任务卡片   │  │   分享截图生成       │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │鹤屋商店占位  │  │  动画层     │  │   useQuestStore     │  │
│  │   页面      │  │ (confetti)  │  │   (Pinia状态管理)    │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
└──────────────────────────┬───────────────────────────────────┘
                           │ Axios / RESTful JSON
                           ▼
┌──────────────────────────────────────────────────────────────┐
│                        后端 (C++ / httplib)                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │QuestService │  │QuestReward  │  │ QuestScheduler      │  │
│  │  任务业务   │  │  奖励发放   │  │ 定时刷新(daily/weekly)│ │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
│  ┌─────────────┐  ┌─────────────┐                            │
│  │UserService  │  │Middleware   │                            │
│  │  用户鉴权   │  │ JWT验证     │                            │
│  └─────────────┘  └─────────────┘                            │
└──────────────────────────┬───────────────────────────────────┘
                           │ SQLite3
                           ▼
┌──────────────────────────────────────────────────────────────┐
│                        数据库 (SQLite3)                       │
│   quests (任务定义) | user_quests (用户进度) | users (积分)   │
└──────────────────────────────────────────────────────────────┘
```

### 2.2 与现有系统的集成点
| 集成点 | 说明 |
|--------|------|
| 用户鉴权 | 复用现有 JWT Token 机制，`Authorization: Bearer <token>` |
| 用户积分 | 扩展 `users` 表新增 `quest_points` 字段 |
| 签到系统 | 预留接口：`/api/quests/checkin-trigger`，供后续签到系统调用 |
| 角色互动 | 预留接口：`/api/quests/interact-trigger`，供好感度系统调用 |

---

## 3. 数据库设计

### 3.1 实体关系

```
users ||--o{ user_quests : has
quests ||--o{ user_quests : tracked_by
```

### 3.2 表结构

#### 表 1：quests（任务定义表）
| 字段 | 类型 | 约束 | 说明 |
|------|------|------|------|
| id | INTEGER | PK, AUTOINCREMENT | 任务唯一标识 |
| quest_key | VARCHAR(64) | UNIQUE, NOT NULL | 业务键，如 `daily_visit_club` |
| name | VARCHAR(128) | NOT NULL | 任务显示名称 |
| description | TEXT | | 任务描述 |
| type | VARCHAR(16) | NOT NULL | `daily` / `weekly` / `legendary` |
| category | VARCHAR(32) | | 分类标签 |
| target_progress | INTEGER | NOT NULL, DEFAULT 1 | 完成所需进度值 |
| points_reward | INTEGER | NOT NULL, DEFAULT 0 | 贡献值奖励 |
| unlock_quest_id | INTEGER | FK → quests.id, NULLABLE | 前置解锁任务（传说任务用） |
| sort_order | INTEGER | DEFAULT 0 | 显示排序 |
| is_active | INTEGER | DEFAULT 1 | 是否启用（0=禁用） |
| created_at | DATETIME | DEFAULT CURRENT_TIMESTAMP | |

**初始化数据（Seed）**
```sql
-- 每日委托
INSERT INTO quests (quest_key, name, description, type, target_progress, points_reward, sort_order) VALUES
('daily_checkin', '文艺社教室巡查', '完成每日签到', 'daily', 1, 100, 1),
('daily_interact_mikuru', '给实玖瑠拍照', '与实玖瑠互动3次', 'daily', 3, 150, 2),
('daily_view_yuki', '长门的读书时间', '查看长门角色详情页', 'daily', 1, 100, 3),
('daily_brainstorm', '团长的突发奇想', '完成春日团长指定的随机任务', 'daily', 1, 200, 4);

-- 周常挑战
INSERT INTO quests (quest_key, name, description, type, target_progress, points_reward, sort_order) VALUES
('weekly_checkin_streak', '无尽八月挑战', '连续7天登录签到', 'weekly', 7, 1000, 10),
('weekly_interact_all', 'SOS团全员集合', '与所有5位核心成员互动至少1次', 'weekly', 5, 800, 11),
('weekly_wall_posts', '电影拍摄周', '在留言墙发布3条以上评论', 'weekly', 3, 600, 12),
('weekly_view_details', '封闭空间调查', '累计查看角色详情10次', 'weekly', 10, 500, 13);

-- 传说任务
INSERT INTO quests (quest_key, name, description, type, target_progress, points_reward, sort_order) VALUES
('legend_first_login', 'SOS团创立', '完成首次登录', 'legendary', 1, 0, 20),
('legend_interact_haruhi', '我对普通人类没有兴趣', '首次与春日互动', 'legendary', 1, 500, 21),
('legend_interact_yuki', '资讯操作', '首次与长门互动', 'legendary', 1, 500, 22),
('legend_interact_mikuru', '这是禁止事项', '首次与实玖瑠互动', 'legendary', 1, 500, 23),
('legend_interact_koizumi', '机关的试探', '首次与古泉互动', 'legendary', 1, 500, 24),
('legend_interact_kyon', '吐槽能量MAX', '首次与阿虚互动', 'legendary', 1, 500, 25),
('legend_winter_login', '凉宫春日的消失', '在冬季（12月）登录满10天', 'legendary', 10, 0, 26),
('legend_daily_master', '凉宫春日的惊愕', '连续30天完成所有每日委托', 'legendary', 30, 5000, 27);
```

#### 表 2：user_quests（用户任务进度表）
| 字段 | 类型 | 约束 | 说明 |
|------|------|------|------|
| id | INTEGER | PK, AUTOINCREMENT | |
| user_id | INTEGER | NOT NULL, FK → users.id | |
| quest_id | INTEGER | NOT NULL, FK → quests.id | |
| status | VARCHAR(16) | NOT NULL, DEFAULT 'available' | `locked`/`available`/`in_progress`/`completed`/`claimed` |
| current_progress | INTEGER | NOT NULL, DEFAULT 0 | 当前进度 |
| unlocked_at | DATETIME | | 解锁时间 |
| completed_at | DATETIME | | 完成时间（达到 target_progress） |
| claimed_at | DATETIME | | 领取奖励时间 |
| created_at | DATETIME | DEFAULT CURRENT_TIMESTAMP | |
| updated_at | DATETIME | DEFAULT CURRENT_TIMESTAMP | |

**索引**：
```sql
CREATE INDEX idx_user_quests_user ON user_quests(user_id);
CREATE INDEX idx_user_quests_status ON user_quests(status);
CREATE UNIQUE INDEX idx_user_quests_unique ON user_quests(user_id, quest_id);
```

#### 表 3：users（扩展字段）
在现有 `users` 表上增加一列：
```sql
ALTER TABLE users ADD COLUMN quest_points INTEGER NOT NULL DEFAULT 0;
```

#### 表 4：daily_quest_reset（每日任务刷新记录 —— 可选，用于审计）
| 字段 | 类型 | 约束 | 说明 |
|------|------|------|------|
| id | INTEGER | PK | |
| user_id | INTEGER | NOT NULL | |
| reset_date | DATE | NOT NULL | 刷新日期 |
| quest_ids | TEXT | NOT NULL | JSON 数组，当日分配的任务ID |
| created_at | DATETIME | DEFAULT CURRENT_TIMESTAMP | |

### 3.3 春日脑洞任务配置
脑洞任务不单独建表，通过**前端配置 + 后端校验**实现：
- 脑洞任务 ID 固定为 `daily_brainstorm`
- 后端提供一个脑洞任务库（硬编码在 C++ 中或 JSON 配置文件）
- 每日刷新时从库中随机抽取 1 条作为当日脑洞
- 记录到 `daily_quest_reset` 的 `quest_ids` 中以便追溯

脑洞任务库结构（后端 C++ 中定义）：
```cpp
struct BrainstormTask {
    std::string id;           // "find_john_smith"
    std::string title;        // "去寻找约翰·史密斯！"
    std::string description;  // "在页面任意位置点击搜索10次"
    int target;               // 10
    std::string check_type;   // "page_click" / "interact" / "refresh" / "comment"
};
```

---

## 4. API 接口设计

### 4.1 接口清单

| 方法 | 路径 | 说明 | 鉴权 |
|------|------|------|------|
| GET | `/api/quests` | 获取当前用户的全部任务列表 | JWT |
| GET | `/api/quests/progress` | 获取任务进度（供前端轮询或主动刷新） | JWT |
| POST | `/api/quests/progress` | 上报任务进度（如点击、查看等） | JWT |
| POST | `/api/quests/{id}/claim` | 领取已完成任务的奖励 | JWT |
| GET | `/api/quests/points` | 获取当前用户贡献值 | JWT |
| POST | `/api/quests/internal/checkin` | 内部接口：签到系统调用触发任务进度 | JWT |
| POST | `/api/quests/internal/interact` | 内部接口：好感度系统调用触发任务进度 | JWT |

### 4.2 接口详情

#### GET /api/quests
**响应 200：**
```json
{
  "code": 200,
  "data": {
    "daily": {
      "reset_at": "2026-05-31T00:00:00Z",
      "completed_count": 2,
      "total_count": 4,
      "quests": [
        {
          "id": 1,
          "quest_key": "daily_checkin",
          "name": "文艺社教室巡查",
          "description": "完成每日签到",
          "type": "daily",
          "status": "claimed",
          "current_progress": 1,
          "target_progress": 1,
          "points_reward": 100,
          "completed_at": "2026-05-31T08:30:00Z",
          "claimed_at": "2026-05-31T08:30:05Z"
        },
        {
          "id": 4,
          "quest_key": "daily_brainstorm",
          "name": "团长的突发奇想",
          "description": "证明1+1=3（完成一道简单数学题）",
          "type": "daily",
          "status": "in_progress",
          "current_progress": 0,
          "target_progress": 1,
          "points_reward": 200
        }
      ]
    },
    "weekly": { ... },
    "legendary": { ... },
    "total_points": 1850
  }
}
```

#### POST /api/quests/progress
**请求体：**
```json
{
  "quest_key": "daily_interact_mikuru",
  "increment": 1
}
```
**响应 200（进度更新后）：**
```json
{
  "code": 200,
  "data": {
    "quest_id": 2,
    "status": "completed",
    "current_progress": 3,
    "target_progress": 3,
    "completed_at": "2026-05-31T10:15:00Z",
    "is_newly_completed": true
  }
}
```
**说明**：`is_newly_completed` 为 true 时，前端触发完成动画。

#### POST /api/quests/{id}/claim
**响应 200：**
```json
{
  "code": 200,
  "data": {
    "claimed_points": 150,
    "total_points": 2000,
    "claimed_at": "2026-05-31T10:15:30Z"
  }
}
```

#### POST /api/quests/internal/checkin
**说明**：由签到系统内部调用，用户无感知。
```json
{ "user_id": 42, "checkin_date": "2026-05-31" }
```
后端自动查找该用户的 `daily_checkin` 和 `weekly_checkin_streak` 任务并更新进度。

### 4.3 错误码
| HTTP | 业务码 | 含义 |
|------|--------|------|
| 400 | 4001 | 任务不存在 |
| 400 | 4002 | 任务未解锁 |
| 400 | 4003 | 任务未完成，无法领取 |
| 400 | 4004 | 任务奖励已领取 |
| 400 | 4005 | 脑洞任务已过期（非当日） |

---

## 5. 前端组件设计

### 5.1 组件树
```
QuestBoard.vue (主页面 /sos/quests)
├── QuestNav.vue              # 顶部标签页：每日/周常/传说
├── QuestProgressBar.vue      # 今日完成进度条 X/4
├── QuestPointsBadge.vue      # 右上角贡献值浮动显示
├── QuestList.vue             # 任务列表容器
│   └── QuestCard.vue         # 单个任务卡片（可翻转）
│       ├── QuestFront.vue    # 卡片正面：标题/描述/奖励
│       └── QuestBack.vue     # 卡片背面：进度/详情/放弃
├── QuestClaimModal.vue       # 领取奖励弹窗（confetti + 数值跳动）
├── QuestShareCard.vue        # 分享截图生成组件
├── BrainstormModal.vue       # 春日脑洞任务专用弹窗（数学题等互动）
└── TsuruyaShopPlaceholder.vue # 鹤屋商店占位页
```

### 5.2 状态管理 (Pinia)
```typescript
// stores/quests.ts
export const useQuestStore = defineStore('quests', () => {
  const quests = ref<GroupedQuests>({ daily: [], weekly: [], legendary: [] })
  const points = ref(0)
  const dailyCompleted = ref(0)
  const dailyTotal = ref(4)
  const lastReset = ref('')
  const loading = ref(false)

  async function fetchQuests()
  async function reportProgress(questKey: string, increment?: number)
  async function claimReward(questId: number)
  async function fetchPoints()
})
```

### 5.3 关键交互流程

**流程 A：页面加载**
```
用户进入 /sos/quests
  → Pinia store 调用 GET /api/quests
  → 渲染任务列表（按状态排序：completed → in_progress → available → locked）
  → 已完成未领取的任务卡片闪烁金色「！」
```

**流程 B：任务进度上报**
```
用户在成员页与实玖瑠互动
  → 前端调用 reportProgress('daily_interact_mikuru', 1)
  → POST /api/quests/progress
  → 后端更新 current_progress，若达到 target 则改 status=completed
  → 响应中 is_newly_completed=true
  → 前端在 QuestCard 上触发「完成动画」：卡片边框金光 → 出现「领取」按钮
```

**流程 C：领取奖励**
```
用户点击「领取」按钮
  → POST /api/quests/{id}/claim
  → 成功后：
    1. QuestCard 变为灰色归档态，移入底部
    2. 屏幕中央弹出 QuestClaimModal（贡献值 +XX 跳动增长）
    3. confetti 粒子爆发（春日黄 + 彩虹色）
    4. 导航栏 points badge 同步更新
  → 若今日 4 个每日委托全部领取：
    1. 春日头像弹出：「今天也是充满干劲的一天！」
```

**流程 D：分享截图**
```
用户点击「分享」按钮
  → html2canvas 捕获 QuestClaimModal 内容
  → 生成 1200x630 分享图
  → 提供「下载图片」和「复制文本」两个选项
```

### 5.4 路由规划
| 路径 | 组件 | 说明 |
|------|------|------|
| `/sos/quests` | QuestBoard.vue | 任务板主页面 |
| `/sos/quests/share` | QuestShareCard.vue | 分享图独立页面（方便社交媒体分享） |
| `/sos/shop` | TsuruyaShopPlaceholder.vue | 鹤屋商店占位页（商品展示，按钮 disabled） |

---

## 6. 后端模块设计

### 6.1 C++ 类结构
```cpp
// include/quest_service.h
class QuestService {
public:
    explicit QuestService(DatabaseManager& db);

    // 用户接口
    json getUserQuests(int userId);
    json getUserQuestProgress(int userId, int questId);
    json reportProgress(int userId, const std::string& questKey, int increment);
    json claimReward(int userId, int questId);
    int getUserPoints(int userId);

    // 内部接口（供其他系统调用）
    void onUserCheckin(int userId, const std::string& date);
    void onUserInteract(int userId, const std::string& characterId);

    // 定时刷新
    void refreshDailyQuests(int userId);
    void refreshWeeklyQuests(int userId);

private:
    DatabaseManager& db_;
    std::vector<BrainstormTask> brainstormTasks_;

    bool isQuestCompleted(int userId, int questId);
    bool isQuestClaimed(int userId, int questId);
    void unlockDependentQuests(int userId, int completedQuestId);
    BrainstormTask pickBrainstormTask(int userId, const std::string& date);
};
```

### 6.2 定时刷新机制
由于后端是纯 C++ 无常驻进程，采用**懒刷新（Lazy Refresh）**策略：
- 用户请求 `/api/quests` 时，后端检查 `last_reset` 时间戳
- 若当前日期 > `last_reset` 日期，则触发 `refreshDailyQuests()`
- 若当前周一 > `last_reset` 周一，则触发 `refreshWeeklyQuests()`
- 刷新逻辑：删除旧 `daily` 类型 `user_quests` 记录，插入新记录

### 6.3 脑洞任务校验
脑洞任务的前端互动（如做数学题）在前端完成，完成后调用 `reportProgress('daily_brainstorm', 1)`。后端**信任前端上报**，不做复杂校验（因已明确「趣味性优先，不要太难」）。仅需校验：
- 该用户当日的脑洞任务确实是这个 `quest_key`
- 任务状态为 `in_progress` 或 `available`

---

## 7. 开发顺序与里程碑

### Milestone 1：数据库 + 后端基础（Day 1）
- [ ] 创建 `quests` 表并插入 Seed 数据
- [ ] 创建 `user_quests` 表
- [ ] 扩展 `users` 表增加 `quest_points` 字段
- [ ] 实现 `QuestService` 核心类骨架
- [ ] 实现 `GET /api/quests` 接口
- [ ] Postman 测试通过

### Milestone 2：后端完整逻辑（Day 2）
- [ ] 实现 `POST /api/quests/progress` 进度上报
- [ ] 实现 `POST /api/quests/{id}/claim` 奖励领取
- [ ] 实现懒刷新机制（daily/weekly）
- [ ] 实现传说任务的前置解锁逻辑
- [ ] 实现脑洞任务随机抽取
- [ ] 实现内部接口 `/internal/checkin` 和 `/internal/interact`
- [ ] 单元测试：覆盖状态流转全部路径

### Milestone 3：前端页面框架（Day 3）
- [ ] 创建 `QuestBoard.vue` 页面与路由 `/sos/quests`
- [ ] 实现 `QuestNav` 标签页切换
- [ ] 实现 `QuestCard` 组件（正面/背面翻转）
- [ ] 实现 `useQuestStore` Pinia 状态管理
- [ ] 对接后端 `GET /api/quests` 渲染列表
- [ ] 完成响应式布局（桌面/平板/手机）

### Milestone 4：前端交互与动画（Day 4）
- [ ] 对接 `POST /api/quests/progress`（进度上报）
- [ ] 对接 `POST /api/quests/{id}/claim`（领取奖励）
- [ ] 实现 `QuestClaimModal`（贡献值跳动 + confetti）
- [ ] 实现全部领取后的春日祝贺弹窗
- [ ] 实现 `QuestShareCard` 截图生成（html2canvas）
- [ ] 实现鹤屋商店占位页 `/sos/shop`

### Milestone 5：脑洞任务与集成（Day 5）
- [ ] 实现 `BrainstormModal` 组件（数学题互动等）
- [ ] 将任务系统与现有页面集成：
  - 签到成功后自动触发 `daily_checkin` 进度
  - 角色互动后自动触发对应互动任务进度
- [ ] 全流程端到端测试
- [ ] 性能优化（API 响应 < 200ms）

---

## 8. 技术选型

| 层面 | 选型 | 理由 |
|------|------|------|
| 前端状态 | Pinia | 已有技术栈，无需新增依赖 |
| 动画 | 纯 CSS + Canvas confetti | 避免引入 heavy 动画库，保持轻量 |
| 截图生成 | html2canvas | 成熟方案，社区广泛使用 |
| 后端定时 | 懒刷新（请求时触发） | 无常驻进程架构的最优解 |
| 数据存储 | SQLite3 | 复用现有后端方案 |
| 接口风格 | RESTful JSON | 与现有后端保持一致 |

---

## 9. 风险评估与应对

| 风险 | 影响 | 应对策略 |
|------|------|----------|
| 懒刷新导致并发问题（多请求同时触发刷新） | 中 | 数据库层用事务包裹刷新逻辑，或加乐观锁 |
| 脑洞任务库需要频繁更新 | 低 | 硬编码在 C++ 中，更新需重新编译；预留 JSON 配置文件迁移路径 |
| 用户多时 `user_quests` 表膨胀 | 中 | 预估：1000 用户 × 30 任务 = 3 万行，SQLite 完全可承受；若超 10 万行考虑归档 |
| 前端截图生成性能差 | 低 | 限制分享图 DOM 复杂度，使用固定尺寸容器；大流量时降级为纯文本分享 |
| C++ 后端开发周期长于预期 | 高 | 优先完成核心接口（GET + claim），progress 上报可先用 mock 数据支撑前端开发 |

---

## 10. 待确认事项（开发前必须明确）

1. **脑洞任务库规模**：首批提供 10 个还是 20 个脑洞任务？10个
2. **传说任务解锁通知**：解锁新传说任务时，是否推送实时通知（如 toast）还是仅在下一次加载任务板时显示？推送实时解锁通知
3. **鹤屋商店商品数据**：商店商品列表是硬编码在前端，还是从后端接口获取？从后端获取
4. **任务分享文案**：分享图上的固定文案（如「我刚刚完成了SOS团的XXX任务！」）是否需要可配置？暂时不需要
