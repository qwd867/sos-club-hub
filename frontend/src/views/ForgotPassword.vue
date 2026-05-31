<template>
  <div class="auth-container">
    <div class="auth-box">
      <h2>忘记密码</h2>
      <form @submit.prevent="handleForgotPassword">
        <div class="form-group">
          <label>邮箱</label>
          <input v-model="email" type="email" required />
        </div>
        <div class="form-group">
          <label>新密码</label>
          <input v-model="newPassword" type="password" required />
        </div>
        <div class="error" v-if="error">{{ error }}</div>
        <div class="success" v-if="success">{{ success }}</div>
        <button type="submit">重置密码</button>
      </form>
      <div class="links">
        <router-link to="/login">返回登录</router-link>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { useAuthStore } from '../stores/auth.js'

const email = ref('')
const newPassword = ref('')
const error = ref('')
const success = ref('')
const authStore = useAuthStore()

async function handleForgotPassword() {
  error.value = ''
  success.value = ''
  try {
    const result = await authStore.forgotPassword(email.value, newPassword.value)
    if (result.code === 200) {
      success.value = result.data?.message || '密码重置成功'
    } else {
      error.value = result.message
    }
  } catch (e) {
    error.value = e.response?.data?.message || '重置失败'
  }
}
</script>

<style scoped>
.auth-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background: #f5f5f5;
}
.auth-box {
  background: white;
  padding: 2rem;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
  width: 360px;
}
h2 {
  text-align: center;
  margin-bottom: 1.5rem;
}
.form-group {
  margin-bottom: 1rem;
}
label {
  display: block;
  margin-bottom: 0.5rem;
  font-weight: bold;
}
input {
  width: 100%;
  padding: 0.5rem;
  border: 1px solid #ddd;
  border-radius: 4px;
  box-sizing: border-box;
}
button {
  width: 100%;
  padding: 0.75rem;
  background: #ff9800;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 1rem;
}
button:hover {
  background: #f57c00;
}
.error {
  color: red;
  margin-bottom: 1rem;
  text-align: center;
}
.success {
  color: green;
  margin-bottom: 1rem;
  text-align: center;
}
.links {
  text-align: center;
  margin-top: 1rem;
}
</style>
