import React from 'react';
import GPBase from './GPBase';

class GPLabel extends GPBase {
    constructor(props) {
        super(props);
        this.clickHandler = this.clickHandler.bind(this);
    }

    render() {
    	return (
    		<text x={this.props.x1} y={this.props.y1} onClick={this.clickHandler}>{this.props.value}</text>
    	);
    }
}

export default GPLabel;