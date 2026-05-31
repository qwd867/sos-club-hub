import axios from 'axios'

const apiClient = axios.create({
  baseURL: 'http://localhost:8080/api',
  headers: {
    'Content-Type': 'application/json'
  }
})

apiClient.interceptors.request.use((config) => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = `Bearer ${token}`
  }
  return config
})

export default {
  register(email, password, username) {
    return apiClient.post('/auth/register', { email, password, username })
  },
  login(email, password) {
    return apiClient.post('/auth/login', { email, password })
  },
  forgotPassword(email, newPassword) {
    return apiClient.post('/auth/forgot-password', { email, new_password: newPassword })
  },
  logout() {
    return apiClient.post('/auth/logout')
  },
  demo() {
    return apiClient.get('/demo')
  }
}
