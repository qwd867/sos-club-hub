<template>
  <div class="quest-page">
    <div class="quest-bg"></div>

    <!-- 顶部导航 -->
    <nav class="quest-nav">
      <div class="nav-left">
        <button class="back-btn" @click="goBack">
          <span class="arrow">←</span> 返回
        </button>
        <h1 class="page-title">春日团长的异世界任务板</h1>
      </div>
      <div class="nav-right">
        <div class="points-badge">
          <span class="points-icon">SOS</span>
          <span class="points-value">{{ questStore.points }}</span>
        </div>
      </div>
    </nav>

    <!-- 内容区 -->
    <main class="quest-main">
      <!-- 进度条 -->
      <div class="progress-section">
        <div class="progress-info">
          <span class="progress-label">今日委托进度</span>
          <span class="progress-count">{{ questStore.dailyCompleted }} / {{ questStore.dailyTotal }}</span>
        </div>
        <div class="progress-track">
          <div
            class="progress-fill"
            :style="{ width: questStore.dailyProgress + '%' }"
          ></div>
        </div>
      </div>

      <!-- 标签页 -->
      <div class="tab-bar">
        <button
          v-for="tab in tabs"
          :key="tab.key"
          class="tab-btn"
          :class="{ active: activeTab === tab.key }"
          @click="activeTab = tab.key"
        >
          {{ tab.label }}
          <span v-if="tab.key === 'daily'" class="tab-count">
            {{ questStore.dailyCompleted }}/{{ questStore.dailyTotal }}
          </span>
        </button>
      </div>

      <!-- 任务列表 -->
      <div class="quest-list" v-if="!questStore.loading">
        <QuestCard
          v-for="quest in currentQuests"
          :key="quest.id"
          :quest="quest"
          @claim="handleClaim"
        />
        <div v-if="currentQuests.length === 0" class="empty-state">
          <p>暂无任务</p>
        </div>
      </div>
      <div v-else class="loading-state">
        <p>加载中...</p>
      </div>

      <!-- 商店入口 -->
      <div class="shop-section">
        <router-link to="/sos/shop" class="shop-link">
          <span class="shop-icon">🏪</span>
          <span>前往鹤屋商店</span>
          <span class="arrow">→</span>
        </router-link>
      </div>
    </main>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { useQuestStore } from '../stores/quests.js'
import QuestCard from '../components/quests/QuestCard.vue'

const router = useRouter()
const questStore = useQuestStore()
const activeTab = ref('daily')

const tabs = [
  { key: 'daily', label: '每日委托' },
  { key: 'weekly', label: '周常挑战' },
  { key: 'legendary', label: '传说任务' },
]

const currentQuests = computed(() => {
  if (activeTab.value === 'daily') return questStore.dailyQuests
  if (activeTab.value === 'weekly') return questStore.weeklyQuests
  return questStore.legendaryQuests
})

function goBack() {
  router.push('/sos')
}

async function handleClaim(questId) {
  try {
    await questStore.claimReward(questId)
  } catch (e) {
    // 错误已在 store 中处理
  }
}

onMounted(() => {
  questStore.fetchQuests()
})
</script>

<style scoped>
.quest-page {
  position: fixed;
  inset: 0;
  overflow-y: auto;
  overflow-x: hidden;
  font-family: 'PingFang SC', 'Microsoft YaHei', 'Helvetica Neue', Helvetica, Arial, sans-serif;
}

.quest-bg {
  position: fixed;
  inset: 0;
  z-index: -1;
  background: linear-gradient(135deg, #fff8e0 0%, #fff0f5 30%, #f0f4ff 60%, #e8f5e9 100%);
  background-size: 400% 400%;
  animation: gradientFlow 20s ease infinite;
}

@keyframes gradientFlow {
  0% { background-position: 0% 50%; }
  50% { background-position: 100% 50%; }
  100% { background-position: 0% 50%; }
}

/* 导航 */
.quest-nav {
  position: sticky;
  top: 0;
  z-index: 100;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 14px 32px;
  max-width: 1000px;
  margin: 0 auto;
  background: rgba(255, 255, 255, 0.75);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border-bottom: 1px solid rgba(255, 255, 255, 0.8);
}

.nav-left {
  display: flex;
  align-items: center;
  gap: 16px;
}

.back-btn {
  padding: 6px 14px;
  border: none;
  border-radius: 20px;
  background: rgba(0, 0, 0, 0.05);
  color: #666;
  font-size: 13px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;
  display: flex;
  align-items: center;
  gap: 4px;
}

.back-btn:hover {
  background: rgba(0, 0, 0, 0.1);
  transform: translateX(-2px);
}

.page-title {
  font-size: 20px;
  font-weight: 800;
  color: #2c2c2c;
  margin: 0;
  background: linear-gradient(135deg, #b45309, #d97706);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
}

.nav-right {
  display: flex;
  align-items: center;
}

.points-badge {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 6px 14px;
  border-radius: 20px;
  background: linear-gradient(135deg, rgba(251, 191, 36, 0.15), rgba(245, 158, 11, 0.1));
  border: 1px solid rgba(245, 158, 11, 0.2);
}

.points-icon {
  font-size: 9px;
  font-weight: 800;
  color: #fff;
  background: linear-gradient(135deg, #f59e0b, #d97706);
  padding: 2px 5px;
  border-radius: 4px;
}

.points-value {
  font-size: 15px;
  font-weight: 700;
  color: #b45309;
}

/* 主内容 */
.quest-main {
  max-width: 900px;
  margin: 0 auto;
  padding: 24px 24px 60px;
}

/* 进度条 */
.progress-section {
  background: rgba(255, 255, 255, 0.8);
  border-radius: 16px;
  padding: 20px 24px;
  margin-bottom: 24px;
  border: 1px solid rgba(0, 0, 0, 0.05);
}

.progress-info {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 10px;
}

.progress-label {
  font-size: 14px;
  font-weight: 700;
  color: #374151;
}

.progress-count {
  font-size: 14px;
  font-weight: 700;
  color: #f59e0b;
}

.progress-track {
  height: 8px;
  border-radius: 4px;
  background: rgba(0, 0, 0, 0.05);
  overflow: hidden;
}

.progress-fill {
  height: 100%;
  border-radius: 4px;
  background: linear-gradient(90deg, #fbbf24, #f59e0b);
  transition: width 0.6s ease;
}

/* 标签页 */
.tab-bar {
  display: flex;
  gap: 8px;
  margin-bottom: 20px;
  padding: 4px;
  background: rgba(255, 255, 255, 0.6);
  border-radius: 14px;
  border: 1px solid rgba(0, 0, 0, 0.05);
}

.tab-btn {
  flex: 1;
  padding: 10px 16px;
  border: none;
  border-radius: 10px;
  background: transparent;
  color: #6b7280;
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
}

.tab-btn:hover {
  background: rgba(0, 0, 0, 0.03);
}

.tab-btn.active {
  background: rgba(255, 255, 255, 0.95);
  color: #1f2937;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.06);
}

.tab-count {
  font-size: 11px;
  padding: 1px 6px;
  border-radius: 8px;
  background: rgba(245, 158, 11, 0.15);
  color: #b45309;
}

/* 任务列表 */
.quest-list {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
  gap: 16px;
  margin-bottom: 32px;
}

.empty-state,
.loading-state {
  grid-column: 1 / -1;
  text-align: center;
  padding: 60px;
  color: #9ca3af;
  font-size: 15px;
}

/* 商店入口 */
.shop-section {
  text-align: center;
}

.shop-link {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 12px 28px;
  border-radius: 30px;
  background: linear-gradient(135deg, #22c55e, #16a34a);
  color: #fff;
  font-size: 15px;
  font-weight: 700;
  text-decoration: none;
  transition: all 0.3s;
  box-shadow: 0 4px 15px rgba(34, 197, 94, 0.3);
}

.shop-link:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(34, 197, 94, 0.4);
}

.shop-icon {
  font-size: 18px;
}

/* 响应式 */
@media (max-width: 768px) {
  .quest-nav {
    padding: 10px 16px;
  }
  .page-title {
    font-size: 16px;
  }
  .quest-main {
    padding: 16px 16px 40px;
  }
  .quest-list {
    grid-template-columns: 1fr;
  }
  .nav-left {
    gap: 10px;
  }
}

@media (max-width: 480px) {
  .back-btn span:not(.arrow) {
    display: none;
  }
  .tab-btn {
    font-size: 12px;
    padding: 8px 10px;
  }
}
</style>
