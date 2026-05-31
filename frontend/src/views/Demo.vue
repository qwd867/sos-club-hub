<template>
  <div class="demo-container">
    <div class="header">
      <h1>Demo 页面</h1>
      <button @click="handleLogout">登出</button>
    </div>
    <div class="content">
      <p>欢迎，{{ authStore.user?.username || authStore.user?.email || '用户' }}！</p>
      <p>这是登录后的 Demo 页面，留作后续扩展。</p>
      <div class="info-box" v-if="message">
        <p>{{ message }}</p>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth.js'
import authApi from '../api/auth.js'

const router = useRouter()
const authStore = useAuthStore()
const message = ref('')

onMounted(async () => {
  try {
    const res = await authApi.demo()
    if (res.data.code === 200) {
      message.value = res.data.data.message
    }
  } catch (e) {
    message.value = '获取演示数据失败'
  }
})

function handleLogout() {
  authStore.logout()
  router.push('/login')
}
</script>

<style scoped>
.demo-container {
  max-width: 800px;
  margin: 0 auto;
  padding: 2rem;
}
.header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  border-bottom: 1px solid #ddd;
  padding-bottom: 1rem;
  margin-bottom: 2rem;
}
button {
  padding: 0.5rem 1rem;
  background: #f44336;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}
button:hover {
  background: #d32f2f;
}
.content {
  background: #fafafa;
  padding: 2rem;
  border-radius: 8px;
}
.info-box {
  margin-top: 1rem;
  padding: 1rem;
  background: #e3f2fd;
  border-radius: 4px;
}
</style>
