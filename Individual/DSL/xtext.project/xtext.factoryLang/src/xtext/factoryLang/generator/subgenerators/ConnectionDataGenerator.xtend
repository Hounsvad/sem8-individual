package xtext.factoryLang.generator.subgenerators

import org.eclipse.xtext.generator.IFileSystemAccess2
import xtext.factoryLang.factoryLang.SsidClass
import xtext.factoryLang.factoryLang.PassClass
import xtext.factoryLang.factoryLang.MqttClass

class ConnectionDataGenerator {
	def static generate(IFileSystemAccess2 fsa, SsidClass ssid, PassClass pass, MqttClass mqtt) {
		fsa.generateFile('Connectiondata/wifi-credentials.config',
			'''
				ssid=«ssid.ssid»
				pass=«pass.pass»
				mqtt=«mqtt.mqtt»
			'''
		)
	}
}