{
"Reservoir": {
		"Storage": {
			"type": "balance",		
			"massbalance": "true",
			"flow": "flow",
			"includeinoutput": "true"
		},
		"reservoir_power": {
			"type": "value"
		},
		"reservoir_coeff": {
			"type": "value"
		}
	},
"Reservoir_link": {
		"flow": {
			"type": "expression",		
			"expression": "reservoir_coeff.s*Storage.s^reservoir_power.s",
			"includeinoutput": "true"
		}
	},
"Reservoir_2": {
		"Storage": {
			"type": "balance",		
			"massbalance": "true",
			"flow": "flow",
			"includeinoutput": "true"
		},
		"flow": {
			"type": "expression",		
			"expression": "reservoir_coeff.s*Storage.s^1.5",
			"includeinoutput": "true"
		},
		"reservoir_coeff": {
			"type": "value"
		}
	}

}
