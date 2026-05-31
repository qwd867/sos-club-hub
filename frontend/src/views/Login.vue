<template>
  <div class="login-page">
    <div class="bg-image"></div>
    <div class="bg-overlay"></div>

    <div class="particles">
      <span v-for="n in 30" :key="n" :style="getParticleStyle(n)"></span>
    </div>

    <div class="login-wrapper">
      <div class="login-card">
        <div class="avatar-ring">
          <img
            src="https://storage.moegirl.org.cn/moegirl/commons/c/ca/Haruhi2.jpg"
            alt="凉宫春日"
            class="avatar"
          />
        </div>

        <h1 class="title">欢迎回来</h1>
        <p class="subtitle">SOS团等候多时了</p>

        <form @submit.prevent="handleLogin" class="login-form">
          <div class="input-group">
            <input
              v-model="email"
              type="email"
              id="email"
              required
              placeholder=" "
            />
            <label for="email">邮箱</label>
            <div class="input-line"></div>
          </div>

          <div class="input-group">
            <input
              v-model="password"
              type="password"
              id="password"
              required
              placeholder=" "
            />
            <label for="password">密码</label>
            <div class="input-line"></div>
          </div>

          <transition name="fade">
            <div class="error-message" v-if="error">
              <span class="error-icon">!</span>
              {{ error }}
            </div>
          </transition>

          <button type="submit" class="login-btn" :class="{ loading: isLoading }">
            <span v-if="!isLoading">登 录</span>
            <span v-else class="spinner"></span>
          </button>
        </form>

        <div class="links">
          <router-link to="/register" class="link">
            <span class="link-icon">+</span>
            注册账号
          </router-link>
          <router-link to="/forgot-password" class="link">
            忘记密码？
          </router-link>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth.js'

const email = ref('')
const password = ref('')
const error = ref('')
const isLoading = ref(false)
const router = useRouter()
const authStore = useAuthStore()

function getParticleStyle(n) {
  const rand = (seed) => {
    const x = Math.sin(seed) * 10000
    return x - Math.floor(x)
  }
  const left = rand(n * 1.1) * 100
  const delay = rand(n * 2.3) * 15
  const duration = 10 + rand(n * 3.7) * 10
  const size = 2 + rand(n * 4.1) * 6
  return {
    left: `${left}%`,
    animationDelay: `${delay}s`,
    animationDuration: `${duration}s`,
    width: `${size}px`,
    height: `${size}px`,
    opacity: 0.3 + rand(n * 5.9) * 0.5
  }
}

async function handleLogin() {
  error.value = ''
  isLoading.value = true
  try {
    const result = await authStore.login(email.value, password.value)
    if (result.code === 200) {
      router.push('/demo')
    } else {
      error.value = result.message
    }
  } catch (e) {
    error.value = e.response?.data?.message || '登录失败'
  } finally {
    isLoading.value = false
  }
}
</script>

<style scoped>
/* ===== 全屏容器 ===== */
.login-page {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  overflow: hidden;
  font-family:
    'PingFang SC',
    'Microsoft YaHei',
    'Helvetica Neue',
    Helvetica,
    Arial,
    sans-serif;
}

/* ===== 背景图 ===== */
.bg-image {
  position: absolute;
  inset: 0;
  background: url('/haruhi-bg.jpg') center/cover no-repeat;
  animation: bgZoom 30s ease-in-out infinite alternate;
}

@keyframes bgZoom {
  0% {
    transform: scale(1);
  }
  100% {
    transform: scale(1.1);
  }
}

/* ===== 暗色遮罩 ===== */
.bg-overlay {
  position: absolute;
  inset: 0;
  background:
    linear-gradient(
      135deg,
      rgba(20, 20, 40, 0.75) 0%,
      rgba(40, 20, 60, 0.55) 50%,
      rgba(20, 20, 40, 0.75) 100%
    );
}

/* ===== 粒子效果 ===== */
.particles {
  position: absolute;
  inset: 0;
  pointer-events: none;
  overflow: hidden;
}

.particles span {
  position: absolute;
  bottom: -10px;
  background: rgba(255, 255, 255, 0.8);
  border-radius: 50%;
  animation: floatUp linear infinite;
  box-shadow: 0 0 6px rgba(255, 255, 255, 0.6);
}

@keyframes floatUp {
  0% {
    transform: translateY(0) scale(1);
    opacity: 0;
  }
  10% {
    opacity: 1;
  }
  90% {
    opacity: 0.6;
  }
  100% {
    transform: translateY(-110vh) scale(0.5);
    opacity: 0;
  }
}

/* ===== 居中包装器 ===== */
.login-wrapper {
  position: relative;
  z-index: 10;
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  padding: 20px;
}

/* ===== 玻璃态卡片 ===== */
.login-card {
  width: 100%;
  max-width: 420px;
  padding: 48px 40px 40px;
  border-radius: 24px;
  background: rgba(255, 255, 255, 0.08);
  backdrop-filter: blur(20px) saturate(180%);
  -webkit-backdrop-filter: blur(20px) saturate(180%);
  border: 1px solid rgba(255, 255, 255, 0.18);
  box-shadow:
    0 8px 32px rgba(31, 38, 135, 0.37),
    inset 0 1px 0 rgba(255, 255, 255, 0.1);
  animation: cardFloat 6s ease-in-out infinite;
  text-align: center;
}

@keyframes cardFloat {
  0%,
  100% {
    transform: translateY(0);
  }
  50% {
    transform: translateY(-8px);
  }
}

/* ===== 头像 ===== */
.avatar-ring {
  width: 90px;
  height: 90px;
  margin: 0 auto 20px;
  padding: 4px;
  border-radius: 50%;
  background: linear-gradient(135deg, #f093fb 0%, #f5576c 50%, #ffd1ff 100%);
  box-shadow: 0 0 20px rgba(240, 147, 251, 0.4);
  animation: avatarPulse 3s ease-in-out infinite;
}

@keyframes avatarPulse {
  0%,
  100% {
    box-shadow: 0 0 20px rgba(240, 147, 251, 0.4);
  }
  50% {
    box-shadow: 0 0 30px rgba(240, 147, 251, 0.7);
  }
}

.avatar {
  width: 100%;
  height: 100%;
  border-radius: 50%;
  object-fit: cover;
  border: 3px solid rgba(255, 255, 255, 0.2);
}

/* ===== 标题 ===== */
.title {
  font-size: 28px;
  font-weight: 700;
  color: #fff;
  margin-bottom: 6px;
  letter-spacing: 2px;
  text-shadow: 0 2px 10px rgba(0, 0, 0, 0.3);
}

.subtitle {
  font-size: 14px;
  color: rgba(255, 255, 255, 0.7);
  margin-bottom: 32px;
  letter-spacing: 1px;
}

/* ===== 表单 ===== */
.login-form {
  display: flex;
  flex-direction: column;
  gap: 24px;
}

.input-group {
  position: relative;
}

.input-group input {
  width: 100%;
  padding: 14px 4px 6px;
  background: transparent;
  border: none;
  border-bottom: 2px solid rgba(255, 255, 255, 0.2);
  color: #fff;
  font-size: 16px;
  outline: none;
  transition: border-color 0.3s;
}

.input-group input:focus {
  border-bottom-color: transparent;
}

.input-group label {
  position: absolute;
  left: 4px;
  top: 14px;
  color: rgba(255, 255, 255, 0.6);
  font-size: 16px;
  pointer-events: none;
  transition: all 0.3s ease;
}

.input-group input:focus + label,
.input-group input:not(:placeholder-shown) + label {
  top: -8px;
  font-size: 12px;
  color: #f093fb;
}

.input-line {
  position: absolute;
  bottom: 0;
  left: 0;
  width: 0;
  height: 2px;
  background: linear-gradient(90deg, #f093fb, #f5576c);
  transition: width 0.4s ease;
}

.input-group input:focus ~ .input-line {
  width: 100%;
}

/* ===== 错误提示 ===== */
.error-message {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  padding: 10px 16px;
  background: rgba(245, 87, 108, 0.15);
  border: 1px solid rgba(245, 87, 108, 0.3);
  border-radius: 10px;
  color: #ffb8c1;
  font-size: 13px;
}

.error-icon {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 18px;
  height: 18px;
  background: #f5576c;
  border-radius: 50%;
  color: #fff;
  font-size: 12px;
  font-weight: bold;
}

.fade-enter-active,
.fade-leave-active {
  transition:
    opacity 0.3s,
    transform 0.3s;
}

.fade-enter-from,
.fade-leave-to {
  opacity: 0;
  transform: translateY(-10px);
}

/* ===== 登录按钮 ===== */
.login-btn {
  position: relative;
  width: 100%;
  padding: 14px;
  margin-top: 8px;
  border: none;
  border-radius: 12px;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: #fff;
  font-size: 16px;
  font-weight: 600;
  letter-spacing: 4px;
  cursor: pointer;
  overflow: hidden;
  transition: all 0.3s ease;
  box-shadow: 0 4px 15px rgba(102, 126, 234, 0.4);
}

.login-btn:hover:not(.loading) {
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(102, 126, 234, 0.6);
}

.login-btn:active:not(.loading) {
  transform: translateY(0);
}

.login-btn::before {
  content: '';
  position: absolute;
  top: 0;
  left: -100%;
  width: 100%;
  height: 100%;
  background: linear-gradient(
    90deg,
    transparent,
    rgba(255, 255, 255, 0.3),
    transparent
  );
  transition: left 0.5s;
}

.login-btn:hover::before {
  left: 100%;
}

.login-btn.loading {
  cursor: not-allowed;
  opacity: 0.8;
}

.spinner {
  display: inline-block;
  width: 20px;
  height: 20px;
  border: 2px solid rgba(255, 255, 255, 0.3);
  border-top-color: #fff;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}

/* ===== 链接 ===== */
.links {
  display: flex;
  justify-content: space-between;
  margin-top: 24px;
  padding-top: 20px;
  border-top: 1px solid rgba(255, 255, 255, 0.1);
}

.link {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  color: rgba(255, 255, 255, 0.7);
  font-size: 14px;
  text-decoration: none;
  transition: all 0.3s;
  padding: 4px 8px;
  border-radius: 6px;
}

.link:hover {
  color: #fff;
  background: rgba(255, 255, 255, 0.1);
  text-shadow: 0 0 8px rgba(255, 255, 255, 0.5);
}

.link-icon {
  font-size: 16px;
  font-weight: bold;
}

/* ===== 响应式 ===== */
@media (max-width: 480px) {
  .login-card {
    padding: 36px 24px 32px;
    border-radius: 20px;
  }

  .title {
    font-size: 24px;
  }

  .avatar-ring {
    width: 76px;
    height: 76px;
  }
}
</style>
