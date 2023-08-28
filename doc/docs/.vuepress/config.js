const { description } = require('../../package')

module.exports = {
	/**
	 * Ref：https://v1.vuepress.vuejs.org/config/#title
	 */
	title: 'Yuescript',
	/**
	 * Ref：https://v1.vuepress.vuejs.org/config/#description
	 */
	description: description,

	/**
	 * Extra tags to be injected to the page HTML `<head>`
	 *
	 * ref：https://v1.vuepress.vuejs.org/config/#head
	 */
	head: [
		['meta', { name: 'theme-color', content: '#3eaf7c' }],
		['meta', { name: 'apple-mobile-web-app-capable', content: 'yes' }],
		['meta', { name: 'apple-mobile-web-app-status-bar-style', content: 'black' }],
		['meta', { property: 'og:title', content: 'Yuescript' }],
		['meta', { property: 'og:description', content: description }],
		// Should probably have a og:url but I'm not sure how to add that without it being the same on all pages
		['meta', { property: 'og:type', content: 'website' }],
		['meta', { property: 'og:image', content: '/image/yuescript.png' }],
		['meta', { property: 'og:image:secure_url', content: '/image/yuescript.png' }],
		['meta', { property: 'og:image:type', content: 'image/png' }],
		['meta', { property: 'og:image:width', content: '1200' }],
		['meta', { property: 'og:image:height', content: '1200' }],
		['link', { rel: 'icon', href: '/image/favicon/favicon-16x16.png', sizes: '16x16', type: 'image/png' }],
		['link', { rel: 'icon', href: '/image/favicon/favicon-32x32.png', sizes: '32x32', type: 'image/png' }],
		['link', { rel: 'apple-touch-icon', href: '/image/favicon/apple-touch-icon-180x180.png', sizes: '180x180', type: 'image/png' }],
		['link', { rel: 'android-chrome', href: '/image/favicon/android-chrome-192x192.png', sizes: '192x192', type: 'image/png' }],
		['link', { rel: 'android-chrome', href: '/image/favicon/android-chrome-512x512.png', sizes: '512x512', type: 'image/png' }],
		['script', {}, `window.global = window;`],
		['script', {}, `
			var Module = {
				onRuntimeInitialized: function() {
					window.yue = Module;
					window.Vue.$data.readonly = false;
					window.Vue.$data.info = Module.version();
				}
			};
		`],
		['script', { src: '/js/yuescript.js' }],
	],

	locales: {
		'/': {
			lang: 'en-US',
			title: 'Yuescript',
			description: 'A language that compiles to Lua'
		},

		'/zh/': {
			lang: 'zh-CN',
			title: '月之脚本',
			description: '一门编译到 Lua 的语言'
		}
	},

	/**
	 * Theme configuration, here is the default theme configuration for VuePress.
	 *
	 * ref：https://v1.vuepress.vuejs.org/theme/default-theme-config.html
	 */
	themeConfig: {
		locales: {
			'/': {
				selectText: 'Languages',
				label: 'English',
				repo: '',
				editLinks: false,
				docsDir: '',
				editLinkText: '',
				lastUpdated: false,
				nav: [
					{
						text: 'Document',
						link: '/doc/'
					},
					{
						text: 'Try yue!',
						link: '/try/',
					},
					{
						text: 'Github',
						link: 'https://github.com/pigpigyyy/Yuescript'
					}
				],
			},

			'/zh/': {
				selectText: '选择语言',
				label: '简体中文',
				repo: '',
				editLinks: false,
				docsDir: '',
				editLinkText: '',
				lastUpdated: false,
				nav: [
					{
						text: '文档',
						link: '/zh/doc/'
					},
					{
						text: '试一试!',
						link: '/zh/try/',
					},
					{
						text: 'Github',
						link: 'https://github.com/pigpigyyy/Yuescript'
					}
				],
			},
		}
	},

	/**
	 * Apply plugins，ref：https://v1.vuepress.vuejs.org/zh/plugin/
	 */
	plugins: [
		'@vuepress/plugin-back-to-top',
		'@vuepress/plugin-medium-zoom',
		'~plugins/vue-js-modal.js',
	]
}
