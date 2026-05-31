<template>
  <div class="auth-page">
    <div class="bg-image"></div>
    <div class="bg-overlay"></div>

    <div class="particles">
      <span v-for="n in 30" :key="n" :style="getParticleStyle(n)"></span>
    </div>

    <div class="auth-wrapper">
      <div class="auth-card">
        <div class="avatar-ring">
          <img
            src="https://storage.moegirl.org.cn/moegirl/commons/c/ca/Haruhi2.jpg"
            alt="凉宫春日"
            class="avatar"
          />
        </div>

        <h1 class="auth-title">{{ title }}</h1>
        <p class="auth-subtitle">{{ subtitle }}</p>

        <slot />
      </div>
    </div>
  </div>
</template>

<script setup>
defineProps({
  title: { type: String, required: true },
  subtitle: { type: String, default: '' }
})

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
</script>

<style scoped>
.auth-page {
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

.bg-image {
  position: absolute;
  inset: 0;
  background: url('/haruhi-bg.jpg') center/cover no-repeat;
  animation: bgZoom 30s ease-in-out infinite alternate;
}

@keyframes bgZoom {
  0% { transform: scale(1); }
  100% { transform: scale(1.1); }
}

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
  10% { opacity: 1; }
  90% { opacity: 0.6; }
  100% {
    transform: translateY(-110vh) scale(0.5);
    opacity: 0;
  }
}

.auth-wrapper {
  position: relative;
  z-index: 10;
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  padding: 20px;
}

.auth-card {
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
  0%, 100% { transform: translateY(0); }
  50% { transform: translateY(-8px); }
}

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
  0%, 100% { box-shadow: 0 0 20px rgba(240, 147, 251, 0.4); }
  50% { box-shadow: 0 0 30px rgba(240, 147, 251, 0.7); }
}

.avatar {
  width: 100%;
  height: 100%;
  border-radius: 50%;
  object-fit: cover;
  border: 3px solid rgba(255, 255, 255, 0.2);
}

.auth-title {
  font-size: 28px;
  font-weight: 700;
  color: #fff;
  margin-bottom: 6px;
  letter-spacing: 2px;
  text-shadow: 0 2px 10px rgba(0, 0, 0, 0.3);
}

.auth-subtitle {
  font-size: 14px;
  color: rgba(255, 255, 255, 0.7);
  margin-bottom: 32px;
  letter-spacing: 1px;
}

@media (max-width: 480px) {
  .auth-card {
    padding: 36px 24px 32px;
    border-radius: 20px;
  }
  .auth-title {
    font-size: 24px;
  }
  .avatar-ring {
    width: 76px;
    height: 76px;
  }
}
</style>

<style>
/* ===== 公共表单样式（限定在 .auth-page 内，避免全局污染） ===== */
.auth-page .auth-form {
  display: flex;
  flex-direction: column;
  gap: 24px;
}

.auth-page .input-group {
  position: relative;
}

.auth-page .input-group input {
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

.auth-page .input-group input:focus {
  border-bottom-color: transparent;
}

.auth-page .input-group label {
  position: absolute;
  left: 4px;
  top: 14px;
  color: rgba(255, 255, 255, 0.6);
  font-size: 16px;
  pointer-events: none;
  transition: all 0.3s ease;
}

.auth-page .input-group input:focus + label,
.auth-page .input-group input:not(:placeholder-shown) + label {
  top: -8px;
  font-size: 12px;
  color: #f093fb;
}

.auth-page .input-line {
  position: absolute;
  bottom: 0;
  left: 0;
  width: 0;
  height: 2px;
  background: linear-gradient(90deg, #f093fb, #f5576c);
  transition: width 0.4s ease;
}

.auth-page .input-group input:focus ~ .input-line {
  width: 100%;
}

/* ===== 消息提示 ===== */
.auth-page .msg {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  padding: 10px 16px;
  border-radius: 10px;
  font-size: 13px;
}

.auth-page .msg-error {
  background: rgba(245, 87, 108, 0.15);
  border: 1px solid rgba(245, 87, 108, 0.3);
  color: #ffb8c1;
}

.auth-page .msg-success {
  background: rgba(76, 175, 80, 0.15);
  border: 1px solid rgba(76, 175, 80, 0.3);
  color: #a5d6a7;
}

.auth-page .msg-icon {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 18px;
  height: 18px;
  border-radius: 50%;
  color: #fff;
  font-size: 12px;
  font-weight: bold;
}

.auth-page .msg-error .msg-icon {
  background: #f5576c;
}

.auth-page .msg-success .msg-icon {
  background: #4caf50;
}

/* ===== 按钮 ===== */
.auth-page .auth-btn {
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

.auth-page .auth-btn:hover:not(.loading) {
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(102, 126, 234, 0.6);
}

.auth-page .auth-btn:active:not(.loading) {
  transform: translateY(0);
}

.auth-page .auth-btn::before {
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

.auth-page .auth-btn:hover::before {
  left: 100%;
}

.auth-page .auth-btn.loading {
  cursor: not-allowed;
  opacity: 0.8;
}

.auth-page .spinner {
  display: inline-block;
  width: 20px;
  height: 20px;
  border: 2px solid rgba(255, 255, 255, 0.3);
  border-top-color: #fff;
  border-radius: 50%;
  animation: authSpin 0.8s linear infinite;
}

@keyframes authSpin {
  to {
    transform: rotate(360deg);
  }
}

/* ===== 链接 ===== */
.auth-page .auth-links {
  display: flex;
  justify-content: space-between;
  margin-top: 24px;
  padding-top: 20px;
  border-top: 1px solid rgba(255, 255, 255, 0.1);
}

.auth-page .auth-links.center {
  justify-content: center;
}

.auth-page .auth-link {
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

.auth-page .auth-link:hover {
  color: #fff;
  background: rgba(255, 255, 255, 0.1);
  text-shadow: 0 0 8px rgba(255, 255, 255, 0.5);
}

/* ===== Vue transition ===== */
.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.3s, transform 0.3s;
}
.fade-enter-from,
.fade-leave-to {
  opacity: 0;
  transform: translateY(-10px);
}
</style>
