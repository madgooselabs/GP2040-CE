import React, { Component } from 'react';
import GPBase from './GPBase';

class GPShape extends GPBase {
	constructor(props) {
        super(props);

        this.state = {
            ...this.state,
            strokeValue: "none",
            fillValue: "none",
            filter: "",
        };

        switch (this.props.fill) {
        	case 0:
        		this.state.fillValue = "none";
        		break;
        	case 1:
        		this.state.fillValue = "black";
        		break;
        	case 2:
        		this.state.fillValue = "black";
        		this.state.filter = "url(#invert-filter)";
        		break;
        }

        switch (this.props.stroke) {
        	case 0:
        		this.state.strokeValue = "none";
        		break;
        	case 1:
        		this.state.strokeValue = "black";
        		break;
        	case 2:
        		this.state.strokeValue = "black";
        		this.state.filter = "url(#invert-filter)";
        		break;
        }
	}

    setSelected(selected) {
        this.setState({'selected': selected});

        switch (this.props.stroke) {
        	case 0:
        		this.setState({"strokeValue": (selected ? "red" : "none")});
        		break;
        	case 1:
        		this.setState({"strokeValue": (selected ? "red" : "black")});
        		break;
        	case 2:
        		this.setState({"strokeValue": (selected ? "red" : "black")});
        		break;
        }
    }

	render() {
		switch (this.props.shape) {
			case 0:
				// ellipse
				return (
					<ellipse cx={this.props.x1} cy={this.props.y1} rx={this.props.x2} ry={this.props.y2} fill={this.state.fillValue} filter={this.state.filterValue} stroke={this.state.strokeValue} onClick={this.clickHandler} />
				);
			case 1:
				// square
				return (
					<rect x={this.props.x1} y={this.props.y1} width={this.props.x2-this.props.x1} height={this.props.y2-this.props.y1} filter={this.state.filterValue} fill={this.state.fillValue} stroke={this.state.strokeValue} onClick={this.clickHandler} />
				);
			case 2:
				// diamond
				return (
					<rect x={this.props.x1-this.props.x2} y={this.props.y1-this.props.y2} width={this.props.x2*2} height={this.props.y2*2} transform={`rotate(45 ${this.props.x1+this.props.x2} ${this.props.y1+this.props.y2})`} filter={this.state.filterValue} fill={this.state.fillValue} stroke={this.state.strokeValue} onClick={this.clickHandler} />
				);
			case 3:
				// polygon
				let points = "";
				let angleIncrement = 2 * Math.PI / this.props.y2;
				for (let i = 0; i < this.props.y2; i++) {
					let angle = i * angleIncrement + this.props.angleStart;
					let x = this.props.x1 + Math.round(this.props.x2 * Math.cos(angle));
					let y = this.props.y1 + Math.round(this.props.x2 * Math.sin(angle));
					points += x + "," + y + " ";
				}

				return (
					<polygon points={points.trim()}  filter={this.state.filterValue} fill={this.state.fillValue} stroke={this.state.strokeValue} onClick={this.clickHandler} />
				);
			case 4:
				// arc
				const startAngle = (this.props.angleStart * Math.PI) / 180;
				const endAngle = (this.props.angleEnd * Math.PI) / 180;
				const startX = this.props.x1 + this.props.x2 * Math.cos(startAngle);
				const startY = this.props.y1 + this.props.y2 * Math.sin(startAngle);
				const endX = this.props.x1 + this.props.x2 * Math.cos(endAngle);
				const endY = this.props.y1 + this.props.y2 * Math.sin(endAngle);

				return (
					<path d={`M${startX},${startY} A${this.props.x2},${this.props.y2} 0 0 1 ${endX},${endY} L${this.props.x1},${this.props.y1} Z`} filter={this.state.filterValue} fill={this.state.fillValue} stroke={this.state.strokeValue} onClick={this.clickHandler} />
				);
			default:
				// nothing
		}
	}
}

export default GPShape;