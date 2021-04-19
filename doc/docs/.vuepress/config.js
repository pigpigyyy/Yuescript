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
