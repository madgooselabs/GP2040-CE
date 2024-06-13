import React, { Component } from 'react';
import GPBase from './GPBase';
import GPShape from './GPShape';

const DPAD_MODES = [
	{ label: 'dpad', value: 0 },
	{ label: 'leftanalog', value: 1 },
	{ label: 'rightanalog', value: 2 },
];

class GPLever extends GPBase {
    constructor(props) {
        super(props);
        this.clickHandler = this.clickHandler.bind(this);
    }

    clickHandler(e) {
        this.props.clickEvent(e, this);
	}

    getDetails() {
        let details = {
            elementType: 'lever',
            leverType: '',
            leverValue: '',
        };

        console.dir(this.props.elementDetails);

        if (this.props.elementDetails) {
            details.leverType = DPAD_MODES.find((o,i) => o.value == this.props.value).label;
            details.leverValue = this.props.value;
        }

        return details;
    }

    render() {
        return (
        	<g onClick={this.clickHandler}>
        		<GPShape
        			x1={this.props.x1}
        			y1={this.props.y1}
        			x2={this.props.x2}
        			y2={this.props.y2}
        			stroke={this.props.stroke}
        			fill={this.props.fill}
        			value={this.props.value}
        			shape={this.props.shape}
        			angleStart={this.props.angleStart}
        			angleEnd={this.props.angleEnd}
        			closed={this.props.closed}
        		/>
        		<GPShape
        			x1={this.props.x1}
        			y1={this.props.y1}
        			x2={this.props.x2*0.75}
        			y2={this.props.y2*0.75}
        			stroke={this.props.stroke}
        			fill={1}
        			value={this.props.value}
        			shape={this.props.shape}
        			angleStart={this.props.angleStart}
        			angleEnd={this.props.angleEnd}
        			closed={this.props.closed}
        		/>
        	</g>
        );
    }
}

export default GPLever;