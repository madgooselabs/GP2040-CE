import React from 'react';
import GPBase from './GPBase';

class GPSprite extends GPBase {
    constructor(props) {
        super(props);
        this.clickHandler = this.clickHandler.bind(this);
    }

    render() {
        return (
            <image x={this.props.x1} y={this.props.y1} width={this.props.x2} height={this.props.y2} href={`#${this.props.value}`} />
        );    
    }
}

export default GPSprite;