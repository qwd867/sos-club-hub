<template>
  <div class="shop-page">
    <div class="shop-bg"></div>

    <!-- 顶部导航 -->
    <nav class="shop-nav">
      <div class="nav-left">
        <button class="back-btn" @click="goBack">
          <span class="arrow">←</span> 返回
        </button>
        <h1 class="page-title">鹤屋商店</h1>
      </div>
      <div class="nav-right">
        <div class="points-badge">
          <span class="points-icon">SOS</span>
          <span class="points-value">{{ questStore.points }}</span>
        </div>
      </div>
    </nav>

    <main class="shop-main">
      <!-- 欢迎横幅 -->
      <div class="shop-banner">
        <div class="banner-avatar">
          <img src="/characters/tsuruya.png" alt="tsuruya" />
        </div>
        <div class="banner-text">
          <h2>哇哈哈！欢迎光临鹤屋商店！</h2>
          <p>鹤屋家什么都有哦～用SOS团贡献值兑换各种好东西吧！</p>
        </div>
      </div>

      <!-- 分类标签 -->
      <div class="category-tabs">
        <button
          v-for="cat in categories"
          :key="cat.key"
          class="tab-btn"
          :class="{ active: activeCategory === cat.key }"
          @click="activeCategory = cat.key"
        >
          {{ cat.label }}
        </button>
      </div>

      <!-- 商品列表 -->
      <div class="goods-grid">
        <div v-for="item in filteredItems" :key="item.id" class="goods-card">
          <div class="goods-image" :style="{ background: item.color }">
            <span class="goods-emoji">{{ item.emoji }}</span>
          </div>
          <h3 class="goods-name">{{ item.name }}</h3>
          <p class="goods-desc">{{ item.desc }}</p>
          <div class="goods-footer">
            <div class="price">
              <span class="price-icon">SOS</span>
              <span class="price-value">{{ item.price }}</span>
            </div>
            <button class="buy-btn" disabled>
              即将开放
            </button>
          </div>
        </div>
      </div>

      <!-- 占位提示 -->
      <div class="placeholder-notice">
        <p>🚧 商店功能正在建设中，敬请期待！</p>
        <p class="sub">鹤屋学姐正在清点库存...</p>
      </div>
    </main>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { useQuestStore } from '../stores/quests.js'

const router = useRouter()
const questStore = useQuestStore()
const activeCategory = ref('all')

const categories = [
  { key: 'all', label: '全部' },
  { key: 'skin', label: '角色皮肤' },
  { key: 'title', label: '称号' },
  { key: 'bg', label: '背景' },
  { key: 'item', label: '道具' },
]

const shopItems = [
  { id: 1, name: '长门的眼镜', desc: '头像装饰', price: 500, emoji: '👓', color: 'linear-gradient(135deg, #a78bfa, #8b5cf6)', category: 'skin' },
  { id: 2, name: '实玖瑠的兔女郎装', desc: '卡片背景', price: 1000, emoji: '🐰', color: 'linear-gradient(135deg, #f472b6, #ec4899)', category: 'skin' },
  { id: 3, name: '春日的冬季校服', desc: '季节限定', price: 2000, emoji: '❄️', color: 'linear-gradient(135deg, #60a5fa, #3b82f6)', category: 'skin' },
  { id: 4, name: '普通人类', desc: '基础称号', price: 300, emoji: '🧑', color: 'linear-gradient(135deg, #9ca3af, #6b7280)', category: 'title' },
  { id: 5, name: '资讯操作者', desc: '稀有称号', price: 500, emoji: '👁️', color: 'linear-gradient(135deg, #a78bfa, #7c3aed)', category: 'title' },
  { id: 6, name: '时间旅者', desc: '稀有称号', price: 500, emoji: '⏳', color: 'linear-gradient(135deg, #fbbf24, #d97706)', category: 'title' },
  { id: 7, name: '超能力者机关员', desc: '稀有称号', price: 500, emoji: '✨', color: 'linear-gradient(135deg, #34d399, #059669)', category: 'title' },
  { id: 8, name: '吐槽星人', desc: '稀有称号', price: 800, emoji: '💬', color: 'linear-gradient(135deg, #f87171, #dc2626)', category: 'title' },
  { id: 9, name: '约翰·史密斯', desc: '隐藏称号', price: 9999, emoji: '🕵️', color: 'linear-gradient(135deg, #fbbf24, #f59e0b, #ef4444)', category: 'title' },
  { id: 10, name: 'SOS团活动室', desc: '页面背景', price: 1500, emoji: '🏫', color: 'linear-gradient(135deg, #fcd34d, #f59e0b)', category: 'bg' },
  { id: 11, name: '封闭空间', desc: '暗黑系背景', price: 1500, emoji: '🌑', color: 'linear-gradient(135deg, #4b5563, #1f2937)', category: 'bg' },
  { id: 12, name: '北高校门', desc: '经典场景', price: 1000, emoji: '🌸', color: 'linear-gradient(135deg, #f9a8d4, #ec4899)', category: 'bg' },
  { id: 13, name: '补签卡', desc: '签到系统用', price: 500, emoji: '📝', color: 'linear-gradient(135deg, #93c5fd, #3b82f6)', category: 'item' },
  { id: 14, name: '好感度双倍卡', desc: '1小时生效', price: 800, emoji: '💕', color: 'linear-gradient(135deg, #f472b6, #db2777)', category: 'item' },
  { id: 15, name: '春日团长的原谅券', desc: '防止降低好感', price: 999, emoji: '🎫', color: 'linear-gradient(135deg, #fbbf24, #f59e0b)', category: 'item' },
]

const filteredItems = computed(() => {
  if (activeCategory.value === 'all') return shopItems
  return shopItems.filter(item => item.category === activeCategory.value)
})

function goBack() {
  router.push('/sos/quests')
}

onMounted(() => {
  questStore.fetchPoints()
})
</script>

<style scoped>
.shop-page {
  position: fixed;
  inset: 0;
  overflow-y: auto;
  overflow-x: hidden;
  font-family: 'PingFang SC', 'Microsoft YaHei', 'Helvetica Neue', Helvetica, Arial, sans-serif;
}

.shop-bg {
  position: fixed;
  inset: 0;
  z-index: -1;
  background: linear-gradient(135deg, #ecfdf5 0%, #f0fdf4 30%, #fffbeb 60%, #fef3c7 100%);
  background-size: 400% 400%;
  animation: gradientFlow 20s ease infinite;
}

@keyframes gradientFlow {
  0% { background-position: 0% 50%; }
  50% { background-position: 100% 50%; }
  100% { background-position: 0% 50%; }
}

.shop-nav {
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
}

.back-btn:hover {
  background: rgba(0, 0, 0, 0.1);
}

.page-title {
  font-size: 20px;
  font-weight: 800;
  color: #2c2c2c;
  margin: 0;
  background: linear-gradient(135deg, #22c55e, #16a34a);
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

.shop-main {
  max-width: 900px;
  margin: 0 auto;
  padding: 24px 24px 60px;
}

/* 欢迎横幅 */
.shop-banner {
  display: flex;
  align-items: center;
  gap: 20px;
  background: rgba(255, 255, 255, 0.85);
  border-radius: 20px;
  padding: 24px;
  margin-bottom: 24px;
  border: 1px solid rgba(0, 0, 0, 0.05);
}

.banner-avatar {
  width: 70px;
  height: 70px;
  border-radius: 50%;
  overflow: hidden;
  flex-shrink: 0;
  border: 3px solid #32CD32;
  box-shadow: 0 4px 15px rgba(50, 205, 50, 0.3);
}

.banner-avatar img {
  width: 100%;
  height: 100%;
  object-fit: cover;
}

.banner-text h2 {
  font-size: 18px;
  font-weight: 700;
  color: #1f2937;
  margin: 0 0 4px;
}

.banner-text p {
  font-size: 13px;
  color: #6b7280;
  margin: 0;
}

/* 分类标签 */
.category-tabs {
  display: flex;
  gap: 8px;
  margin-bottom: 20px;
  padding: 4px;
  background: rgba(255, 255, 255, 0.6);
  border-radius: 14px;
  border: 1px solid rgba(0, 0, 0, 0.05);
  overflow-x: auto;
}

.tab-btn {
  padding: 8px 16px;
  border: none;
  border-radius: 10px;
  background: transparent;
  color: #6b7280;
  font-size: 13px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;
  white-space: nowrap;
}

.tab-btn:hover {
  background: rgba(0, 0, 0, 0.03);
}

.tab-btn.active {
  background: rgba(255, 255, 255, 0.95);
  color: #1f2937;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.06);
}

/* 商品网格 */
.goods-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(180px, 1fr));
  gap: 16px;
  margin-bottom: 32px;
}

.goods-card {
  background: rgba(255, 255, 255, 0.85);
  border-radius: 16px;
  padding: 16px;
  border: 1px solid rgba(0, 0, 0, 0.05);
  transition: all 0.3s;
  display: flex;
  flex-direction: column;
}

.goods-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 12px 30px rgba(0, 0, 0, 0.08);
}

.goods-image {
  width: 60px;
  height: 60px;
  border-radius: 14px;
  display: flex;
  align-items: center;
  justify-content: center;
  margin-bottom: 12px;
}

.goods-emoji {
  font-size: 28px;
}

.goods-name {
  font-size: 15px;
  font-weight: 700;
  color: #1f2937;
  margin: 0 0 4px;
}

.goods-desc {
  font-size: 12px;
  color: #9ca3af;
  margin: 0 0 12px;
}

.goods-footer {
  margin-top: auto;
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.price {
  display: flex;
  align-items: center;
  gap: 3px;
}

.price-icon {
  font-size: 8px;
  font-weight: 800;
  color: #fff;
  background: linear-gradient(135deg, #f59e0b, #d97706);
  padding: 1px 4px;
  border-radius: 3px;
}

.price-value {
  font-size: 13px;
  font-weight: 700;
  color: #b45309;
}

.buy-btn {
  padding: 5px 12px;
  border: none;
  border-radius: 10px;
  background: rgba(0, 0, 0, 0.05);
  color: #9ca3af;
  font-size: 12px;
  font-weight: 600;
  cursor: not-allowed;
}

/* 占位提示 */
.placeholder-notice {
  text-align: center;
  padding: 32px;
  background: rgba(255, 255, 255, 0.6);
  border-radius: 16px;
  border: 2px dashed rgba(0, 0, 0, 0.1);
}

.placeholder-notice p {
  font-size: 15px;
  color: #6b7280;
  margin: 0;
}

.placeholder-notice .sub {
  font-size: 13px;
  color: #9ca3af;
  margin-top: 4px;
}

/* 响应式 */
@media (max-width: 768px) {
  .shop-nav {
    padding: 10px 16px;
  }
  .shop-main {
    padding: 16px 16px 40px;
  }
  .shop-banner {
    flex-direction: column;
    text-align: center;
  }
  .goods-grid {
    grid-template-columns: repeat(2, 1fr);
  }
}

@media (max-width: 480px) {
  .goods-grid {
    grid-template-columns: 1fr;
  }
}
</style>
