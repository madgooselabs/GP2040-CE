import React, { Component } from 'react';

class GPBase extends Component {
    constructor(props) {
        super(props);

        this.state = {
            ...this.state,
            selected: false,
        };
    }

    clickHandler(e) {
        if (this && this.props) {
            this.props.clickEvent(e, this);
        }
	}

    getDetails() {
        return {};
    }

    setSelected(selected) {
        this.setState({'selected': selected});
    }
}

export default GPBase;