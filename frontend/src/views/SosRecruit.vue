<template>
  <div class="sos-page">
    <!-- 背景 -->
    <div class="sos-bg"></div>
    <div class="sos-particles">
      <span v-for="n in 10" :key="n" :style="particleStyle(n)"></span>
    </div>

    <!-- 顶部导航 -->
    <nav class="sos-nav">
      <div class="nav-brand">SOS团</div>
      <button class="logout-btn" @click="handleLogout">
        登出
      </button>
    </nav>

    <!-- Hero -->
    <section class="hero">
      <div class="hero-badge">SOS</div>
      <h1 class="hero-title">让世界变得更热闹的凉宫春日的团</h1>
      <p class="hero-sub">—— 北高一年级神秘组织 · 现在招新中！——</p>
      <button class="hero-cta" @click="scrollToMembers">我要入团</button>
    </section>

    <!-- 成员展示 -->
    <section class="members-section" ref="membersRef">
      <div class="section-header">
        <h2>团内成员</h2>
        <p>点击卡片查看详细档案</p>
      </div>

      <div class="members-grid">
        <MemberCard
          v-for="member in members"
          :key="member.id"
          :member="member"
          @click="openModal(member)"
        />
      </div>
    </section>

    <!-- 页脚 -->
    <footer class="sos-footer">
      <p class="slogan">让世界变得更热闹！</p>
      <p class="copyright">北高文艺社第二活动室 · SOS团非官方网站</p>
    </footer>

    <!-- 详情弹窗 -->
    <MemberModal v-model="modalOpen" :member="selectedMember" />
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth.js'
import MemberCard from '../components/sos/MemberCard.vue'
import MemberModal from '../components/sos/MemberModal.vue'

const router = useRouter()
const authStore = useAuthStore()
const membersRef = ref(null)
const modalOpen = ref(false)
const selectedMember = ref({})

function scrollToMembers() {
  membersRef.value?.scrollIntoView({ behavior: 'smooth' })
}

function openModal(member) {
  selectedMember.value = member
  modalOpen.value = true
}

function handleLogout() {
  authStore.logout()
  router.push('/login')
}

function particleStyle(n) {
  const rand = (seed) => {
    const x = Math.sin(seed) * 10000
    return x - Math.floor(x)
  }
  const colors = ['#FFD700', '#00BFFF', '#FF69B4', '#9370DB', '#20B2AA']
  const color = colors[Math.floor(rand(n * 7.1) * colors.length)]
  return {
    left: `${rand(n * 1.3) * 100}%`,
    top: `${rand(n * 2.7) * 100}%`,
    width: `${3 + rand(n * 4.3) * 6}px`,
    height: `${3 + rand(n * 4.3) * 6}px`,
    background: color,
    animationDelay: `${rand(n * 5.9) * 8}s`,
    animationDuration: `${6 + rand(n * 6.1) * 8}s`,
    opacity: 0.3 + rand(n * 8.3) * 0.4
  }
}

const members = [
  {
    id: 'haruhi',
    name: '凉宫春日',
    role: '团长',
    tag: '超能力发起者',
    color: '#FFD700',
    avatar: '/characters/haruhi.jpg',
    summary: '让全世界都知道她的存在的超活跃女高中生，SOS团的发起人和绝对核心。',
    details: {
      position: '故事的核心发动机，拥有在无意识中改变现实的「神之力」。',
      duty: '制定所有活动计划、强迫团员参与、负责惹麻烦和解决问题。',
      quote: '「我对普通的人类没有兴趣。你们之中要是有外星人、未来人、异世界来的人、超能力者，就尽管来找我吧！」',
      ability: '封闭空间创造、现实改写（无意识）。',
      moe: '团长的绝对自信、对奇怪事物的执着、黑色长发+黄色发带标志性造型。'
    }
  },
  {
    id: 'yuki',
    name: '长门有希',
    role: '文艺社员',
    tag: '资讯统合思念体',
    color: '#9370DB',
    avatar: '/characters/yuki.jpg',
    summary: '北高文艺社唯一社员，被春日强占社团教室后加入SOS团的神秘三无少女。',
    details: {
      position: '资讯统合思念体派遣的外星观察员，负责监视春日。',
      duty: '提供技术支持（电脑/黑客能力）、默默执行各种超现实任务、读书。',
      quote: '「…（沉默）」「这是资讯操作」「我有话要告诉你」',
      ability: '资讯操作（物质改写、空间移动、记忆篡改等）。',
      moe: '无口属性、眼镜娘（初期）、对书的痴迷、微表情变化、在《消失》中的情感爆发。'
    }
  },
  {
    id: 'mikuru',
    name: '朝比奈实玖瑠',
    role: 'SOS团女仆',
    tag: '未来人',
    color: '#FF69B4',
    avatar: '/characters/mikuru.jpg',
    summary: '来自未来的时间旅行者，被春日拉入团后被迫穿上各种cosplay服装的治愈系学姐。',
    details: {
      position: '时间派来的调查员，负责监视春日的时间震动。',
      duty: '泡茶、穿女仆装/护士装/兔女郎装等服侍大家、提供未来视角的信息。',
      quote: '「啊哇哇…」「这是禁止事项」「团长，那个不可以…」',
      ability: '时间跳跃（需借助未来机构设备）。',
      moe: '天然呆、易哭、童颜巨乳、各种cosplay造型、"啊哇哇"口癖。'
    }
  },
  {
    id: 'koizumi',
    name: '古泉一树',
    role: '转学生',
    tag: '超能力者',
    color: '#20B2AA',
    avatar: '/characters/koizumi.jpg',
    summary: '转学第一天就自称超能力者的神秘帅哥，实际上真的是超能力组织的成员。',
    details: {
      position: '超能力组织「机关」派遣的特工，负责在春日的封闭空间中战斗。',
      duty: '和春日一起策划活动（尤其推理类）、在封闭空间与神人战斗、微笑。',
      quote: '「这只是我的个人兴趣」「团长的话，一定可以做到」「嘛，这也是没办法的事」',
      ability: '在春日的封闭空间中发动「光球」攻击神人。',
      moe: '永远保持微笑的腹黑属性、和春日默契配合、对阿虚的哲学对话、转学生设定。'
    }
  },
  {
    id: 'kyon',
    name: '阿虚',
    role: '吐槽役',
    tag: '普通人类',
    color: '#708090',
    avatar: '/characters/kyon.jpg',
    summary: '被春日强行拉入SOS团的普通高中生，负责吐槽和阻止世界毁灭。',
    details: {
      position: '故事的叙述视角，连接四个非人类与日常世界的锚点。',
      duty: '吐槽团长、阻止春日做过火的事情、照顾有希、被实玖瑠治愈、和古泉聊天。',
      quote: '「我是这样想的…（大段内心独白）」「喂，凉宫！」「长门，拜托你了」',
      ability: '无（但似乎是唯一能真正影响春日决定的人）。',
      moe: '超长内心独白、精准吐槽、对妹妹的疼爱、在《消失》中觉醒的意志力。'
    }
  },
  {
    id: 'tsuruya',
    name: '鹤屋学姐',
    role: '赞助人',
    tag: '大财阀千金',
    color: '#32CD32',
    avatar: '/characters/tsuruya.png',
    summary: '实玖瑠的同班同学兼好友，超级有钱的大小姐，SOS团的隐形赞助人。',
    details: {
      position: '鹤屋财团的千金，对SOS团的奇怪活动充满兴趣的旁观者。',
      duty: '提供活动经费和场地（别墅、游艇等）、和春日一起疯、照顾实玖瑠。',
      quote: '「哇哈哈！这很有趣嘛！」「鹤屋家什么都有哦～」',
      ability: '钞能力（字面意义上的超能力）。',
      moe: '爽朗的大笑声、毫无顾忌的性格、对奇怪事物的包容力、和春日旗鼓相当的行动力。'
    }
  },
  {
    id: 'sasaki',
    name: '佐佐木',
    role: '谜之转学生',
    tag: ' another 可能性',
    color: '#6495ED',
    avatar: '/characters/sasaki.jpg',
    summary: '阿虚初中时代的好友，和春日相似却又截然不同的存在，世界的另一种可能性。',
    details: {
      position: '《凉宫春日的消失》中的关键人物，拥有与春日相似却相反的性质。',
      duty: '观察春日、与「机关」对抗派接触、作为阿虚的「正常」选择存在。',
      quote: '「有趣的女人」「阿虚，你和凉宫同学是什么关系？」',
      ability: '与春日相似的现实改写潜力（但未觉醒）。',
      moe: '冷静的知性美、与春日形成鲜明对比的理性、短发造型、对阿虚的特别关注。'
    }
  }
]
</script>

<style scoped>
.sos-page {
  position: fixed;
  inset: 0;
  overflow-y: auto;
  overflow-x: hidden;
  font-family:
    'PingFang SC',
    'Microsoft YaHei',
    'Helvetica Neue',
    Helvetica,
    Arial,
    sans-serif;
}

/* 背景 */
.sos-bg {
  position: fixed;
  inset: 0;
  z-index: -2;
  background: linear-gradient(135deg, #e0e7ff 0%, #f3e8ff 40%, #fff0f5 70%, #fff8e0 100%);
  background-size: 400% 400%;
  animation: gradientFlow 20s ease infinite;
  will-change: background-position;
}

@keyframes gradientFlow {
  0% { background-position: 0% 50%; }
  50% { background-position: 100% 50%; }
  100% { background-position: 0% 50%; }
}

/* 粒子 */
.sos-particles {
  position: fixed;
  inset: 0;
  z-index: -1;
  pointer-events: none;
  overflow: hidden;
}

.sos-particles span {
  position: absolute;
  border-radius: 50%;
  animation: particleFloat linear infinite;
}

@keyframes particleFloat {
  0%, 100% {
    transform: translate(0, 0) scale(1);
  }
  25% {
    transform: translate(20px, -30px) scale(1.2);
  }
  50% {
    transform: translate(-10px, -50px) scale(0.8);
  }
  75% {
    transform: translate(15px, -20px) scale(1.1);
  }
}

/* 导航 */
.sos-nav {
  position: sticky;
  top: 0;
  z-index: 100;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 40px;
  max-width: 1200px;
  margin: 0 auto;
  background: rgba(255, 255, 255, 0.6);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border-bottom: 1px solid rgba(255, 255, 255, 0.8);
}

.nav-brand {
  font-size: 22px;
  font-weight: 800;
  background: linear-gradient(135deg, #FFD700, #FF69B4);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  letter-spacing: 2px;
}

.logout-btn {
  padding: 8px 20px;
  border: none;
  border-radius: 20px;
  background: linear-gradient(135deg, #667eea, #764ba2);
  color: #fff;
  font-size: 13px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s;
  box-shadow: 0 2px 10px rgba(102, 126, 234, 0.3);
}

.logout-btn:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 15px rgba(102, 126, 234, 0.5);
}

/* Hero */
.hero {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  min-height: calc(100vh - 60px);
  padding: 40px 20px;
}

.hero-badge {
  width: 100px;
  height: 100px;
  border-radius: 50%;
  background: linear-gradient(135deg, #FFD700, #FF8C00);
  color: #fff;
  font-size: 32px;
  font-weight: 900;
  display: flex;
  align-items: center;
  justify-content: center;
  margin-bottom: 30px;
  box-shadow: 0 8px 30px rgba(255, 215, 0, 0.4);
  animation: badgePulse 3s ease-in-out infinite;
  text-shadow: 0 2px 4px rgba(0,0,0,0.2);
  letter-spacing: 2px;
}

@keyframes badgePulse {
  0%, 100% { transform: scale(1); box-shadow: 0 8px 30px rgba(255, 215, 0, 0.4); }
  50% { transform: scale(1.05); box-shadow: 0 12px 40px rgba(255, 215, 0, 0.6); }
}

.hero-title {
  font-size: 42px;
  font-weight: 800;
  color: #2c2c2c;
  margin-bottom: 12px;
  letter-spacing: 2px;
  line-height: 1.3;
  max-width: 800px;
}

.hero-sub {
  font-size: 16px;
  color: #888;
  margin-bottom: 36px;
  letter-spacing: 1px;
}

.hero-cta {
  padding: 14px 48px;
  border: none;
  border-radius: 30px;
  background: linear-gradient(135deg, #FFD700, #FF8C00);
  color: #fff;
  font-size: 18px;
  font-weight: 700;
  letter-spacing: 3px;
  cursor: pointer;
  transition: all 0.3s;
  box-shadow: 0 4px 20px rgba(255, 140, 0, 0.35);
  animation: ctaFloat 3s ease-in-out infinite;
}

@keyframes ctaFloat {
  0%, 100% { transform: translateY(0); }
  50% { transform: translateY(-6px); }
}

.hero-cta:hover {
  transform: translateY(-3px) scale(1.05);
  box-shadow: 0 8px 30px rgba(255, 140, 0, 0.5);
}

/* 成员区 */
.members-section {
  padding: 80px 40px;
  max-width: 1200px;
  margin: 0 auto;
}

.section-header {
  text-align: center;
  margin-bottom: 50px;
}

.section-header h2 {
  font-size: 32px;
  font-weight: 800;
  color: #2c2c2c;
  margin-bottom: 10px;
  letter-spacing: 2px;
}

.section-header p {
  font-size: 15px;
  color: #999;
}

.members-grid {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  gap: 28px;
}

.members-grid > * {
  flex: 0 0 220px;
  max-width: 260px;
}

/* 页脚 */
.sos-footer {
  text-align: center;
  padding: 60px 20px 40px;
  border-top: 1px solid rgba(0, 0, 0, 0.06);
}

.slogan {
  font-size: 24px;
  font-weight: 700;
  background: linear-gradient(135deg, #FFD700, #FF69B4);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  margin-bottom: 10px;
  letter-spacing: 3px;
}

.copyright {
  font-size: 13px;
  color: #aaa;
}

/* 响应式 */
@media (max-width: 768px) {
  .hero-title {
    font-size: 28px;
  }
  .hero-badge {
    width: 80px;
    height: 80px;
    font-size: 26px;
  }
  .members-section {
    padding: 50px 20px;
  }
  .members-grid {
    gap: 16px;
  }
  .members-grid > * {
    flex: 0 0 calc(50% - 8px);
    max-width: none;
  }
  .sos-nav {
    padding: 12px 20px;
  }
}

@media (max-width: 480px) {
  .members-grid > * {
    flex: 0 0 100%;
    max-width: 320px;
  }
  .hero-title {
    font-size: 24px;
  }
}
</style>
