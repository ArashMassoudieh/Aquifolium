{
"Reservoir": {
		"Storage": {
			"type": "balance",		
			"massbalance": "true",
			"flow": "flow"
		},
		"flow": {
			"type": "expression",		
			"expression": "reservoir_coeff.s*Storage.s^reservoir_power.s"
		},
		"reservoir_power": {
			"type": "value"
		},
		"reservoir_coeff": {
			"type": "value"
		}
	},
"Reservoir_2": {
		"Storage": {
			"type": "balance",		
			"massbalance": "true",
			"flow": "flow"
		},
		"flow": {
			"type": "expression",		
			"expression": "reservoir_coeff.s*Storage.s^1.5"
		},
		"reservoir_coeff": {
			"type": "value"
		}
	}

}