import axios from 'axios'

const apiClient = axios.create({
  baseURL: 'http://localhost:8080/api',
  timeout: 10000,
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
  getQuests() {
    return apiClient.get('/quests')
  },
  reportProgress(questKey, increment = 1) {
    return apiClient.post('/quests/progress', { quest_key: questKey, increment })
  },
  claimReward(questId) {
    return apiClient.post(`/quests/${questId}/claim`)
  },
  getPoints() {
    return apiClient.get('/quests/points')
  }
}
