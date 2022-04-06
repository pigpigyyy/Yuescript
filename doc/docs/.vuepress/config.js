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
		['meta', { property: 'og:image', content: 'https://yuescript.org/image/yuescript.png' }],
		['meta', { property: 'og:image:secure_url', content: 'https://yuescript.org/image/yuescript.png' }],
		['meta', { property: 'og:image:type', content: 'image/png' }],
		['meta', { property: 'og:image:width', content: '1200' }],
		['meta', { property: 'og:image:height', content: '1200' }],
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
		['script', {}, `
			var Module = {
				onRuntimeInitialized: function() {
					window.yue = Module;
				}
			};
		`],
		['script', { src: '/js/yuescript.js' }],
	],

	/**
	 * Theme configuration, here is the default theme configuration for VuePress.
	 *
	 * ref：https://v1.vuepress.vuejs.org/theme/default-theme-config.html
	 */
	themeConfig: {
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

	/**
	 * Apply plugins，ref：https://v1.vuepress.vuejs.org/zh/plugin/
	 */
	plugins: [
		'@vuepress/plugin-back-to-top',
		'@vuepress/plugin-medium-zoom',
		'~plugins/vue-js-modal.js',
	]
}
