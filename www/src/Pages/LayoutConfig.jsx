import React, { useCallback, useContext, useEffect, useState } from 'react';
import { AppContext } from '../Contexts/AppContext';
import { Button, Form, FormCheck, FormSelect, Table, Accordion } from 'react-bootstrap';
import { Formik, useFormikContext, getIn } from 'formik';
import * as yup from 'yup';
import { useTranslation } from 'react-i18next';
import ContextualHelpOverlay from '../Components/ContextualHelpOverlay';
import CustomSelect from '../Components/CustomSelect';
import Section from '../Components/Section';
import { GPController } from '../Components/ButtonLayouts';
import WebApi from '../Services/WebApi';
import boards from '../Data/Boards.json';
import useLayoutStore from '../Store/useLayoutStore';
import {
	BUTTONS,
} from '../Data/Buttons';
import LEDColors from '../Data/LEDColors';
import './LayoutConfig.scss';

const buttonLayoutSchemaBase = yup.number().required();

let buttonLayoutSchema = buttonLayoutSchemaBase.label('Button Layout Left');
let buttonLayoutRightSchema = buttonLayoutSchemaBase.label(
	'Button Layout Right',
);

let layoutFieldsSchema = {
	buttonLayout: buttonLayoutSchema,
	buttonLayoutRight: buttonLayoutRightSchema,
};
const schema = yup.object().shape({
	...layoutFieldsSchema,
});
const LEDButton = ({
	id,
	name,
	buttonType,
	buttonColor,
	buttonPressedColor,
	className,
	labelUnder,
	elementProperties,
	onClick,
	...props
}) => {
	const [pressed, setPressed] = useState(false);
	const handlePressedShow = (e) => {
		// Show pressed state on right-click
		if (e.button === 2) setPressed(true);
	};
	const handlePressedHide = (e) => {
		// Revert to normal state
		setPressed(false);
	};
	buttonColor = '#000000';
	if (elementProperties) {
		//console.dir(elementProperties);
		let leftValue = 0;
		let topValue = 0;
		let widthValue = 0;
		let heightValue = 0;
		let radiusSize = 0;
		let rotationValue = 0;
		if (elementProperties.parameters.shape == 0) {
			// ellipse
			leftValue = (elementProperties.parameters.x1);
			topValue = (elementProperties.parameters.y1);
			widthValue = (elementProperties.parameters.x2);
			heightValue = (elementProperties.parameters.x2);
			radiusSize = 50;
		} else if (elementProperties.parameters.shape == 1) {
			// square
			leftValue = (elementProperties.parameters.x1);
			topValue = ((elementProperties.parameters.y1));
			widthValue = ((elementProperties.parameters.x2-elementProperties.parameters.x1));
			heightValue = ((elementProperties.parameters.y2-elementProperties.parameters.y1));
			radiusSize = 10;
		} else if (elementProperties.parameters.shape == 2) {
			// diamond
			leftValue = (elementProperties.parameters.x1);
			topValue = ((elementProperties.parameters.y1));
			widthValue = ((elementProperties.parameters.x2*2));
			heightValue = ((elementProperties.parameters.x2*2));
			radiusSize = 10;
			rotationValue = 45;
		} else if (elementProperties.parameters.shape == 3) {
			// polygon
			leftValue = (elementProperties.parameters.x1);
			topValue = (elementProperties.parameters.y1);
			widthValue = (elementProperties.parameters.x2);
			heightValue = (elementProperties.parameters.x2);
			radiusSize = 50;
		} else if (elementProperties.parameters.shape == 4) {
			// arc
		}
		//if ((elementProperties.elementType == 2)||(elementProperties.elementType == 3)||(elementProperties.elementType == 4)||(elementProperties.elementType == 8)) {
		//}
		
		return (
			<div
				className={`led-button ${className}`}
				style={{
					backgroundColor: pressed ? buttonPressedColor : buttonColor, 
					left: leftValue+'mm', 
					top: topValue+'mm',
					width: widthValue+'mm',
					height: heightValue+'mm',
					borderRadius: radiusSize != 0 ? `${radiusSize}%` : '',
					transform: rotationValue != 0 ? `rotate(-${rotationValue}deg)` : null,
				}}
				onClick={onClick}
				onMouseDown={(e) => handlePressedShow(e)}
				onMouseUp={(e) => handlePressedHide(e)}
				onMouseLeave={(e) => handlePressedHide(e)}
				onContextMenu={(e) => e.preventDefault()}
			>
				<span className={`button-label ${labelUnder ? 'under' : ''}`}>
					{name}
				</span>
			</div>
		);
	} else {
		return (
			<div
				className={`led-button ${className}`}
				style={{ backgroundColor: pressed ? buttonPressedColor : buttonColor }}
				onClick={onClick}
				onMouseDown={(e) => handlePressedShow(e)}
				onMouseUp={(e) => handlePressedHide(e)}
				onMouseLeave={(e) => handlePressedHide(e)}
				onContextMenu={(e) => e.preventDefault()}
			>
				<span className={`button-label ${labelUnder ? 'under' : ''}`}>
					{name}
				</span>
			</div>
		);
	}
};
const FormContext = () => {
	const { values, setValues } = useFormikContext();
	const { setLoading } = useContext(AppContext);
	useEffect(() => {
		async function fetchData() {
			const data = await WebApi.getDisplayOptions();
			setValues(data);
		}
		fetchData();
	}, [setValues]);

	useEffect(() => {
		async function getLayoutOptions() {
			if (!!values.buttonLayout) values.buttonLayout = parseInt(values.buttonLayout);
			if (!!values.buttonLayoutRight) values.buttonLayoutRight = parseInt(values.buttonLayoutRight);
		}
        getLayoutOptions();
	}, [values, setValues]);
	return null;
};
export default function LayoutConfigPage() {
	const { setLoading, buttonLabels, setButtonLabels, usedPins } = useContext(AppContext);
	const [saveMessage, setSaveMessage] = useState('');
	const [selectedButton, setSelectedButton] = useState('');
	const { buttonLabelType } = buttonLabels;
	const { fetchLayoutDefinitions, fetchLayoutDetails, buttonLayoutsLeft, buttonLayoutsRight, layoutSettings } = useLayoutStore();

	useEffect(() => {
		fetchLayoutDefinitions();
		fetchLayoutDetails();
	}, []);

	const onSuccess = async (values) => {
		const cleanValues = schema.cast(values);

		//const success = await WebApi.setPeripheralOptions(cleanValues);
		setSaveMessage(
			success
				? t('Common:saved-success-message')
				: t('Common:saved-error-message'),
		);
	};

	const toggleSelectedButton = (e, buttonName) => {
		e.stopPropagation();
		console.dir(selectedButton);
		if (selectedButton === buttonName) {
			//setPickerVisible(false);
		} else {
			//setLedOverlayTarget(e.target);
			//setSelectedButton(buttonName);
			//setSelectedColor(
			//	buttonName === 'ALL' ? '#000000' : customTheme[buttonName].normal,
			//);
			//setPickerType({ type: 'normal', button: buttonName });
			//setPickerVisible(true);
		}
	};

	const getOptionLabelLeft = useCallback(
        (option) => {
            return t(`LayoutConfig:layouts.left.${option.label}`);
        },
        [buttonLayoutsLeft],
	);

	const getOptionLabelRight = useCallback(
        (option) => {
             t(`LayoutConfig:layouts.right.${option.label}`);
        },
        [buttonLayoutsRight],
	);

	const setValueLeft = useCallback(
        (option,values) => {
            values.buttonLayout = buttonLayoutsLeft.find((o) => o.value == option.value).value
        },
        [buttonLayoutsLeft],
	);

	const setValueRight = useCallback(
        (option,values) => {
            values.buttonLayoutRight = buttonLayoutsRight.find((o) => o.value == option.value).value
        },
        [buttonLayoutsRight],
	);

	const { t } = useTranslation('');
	return (
		<Formik onSubmit={onSuccess} validationSchema={schema} initialValues={{}}>
			{({ errors, handleSubmit, handleChange, setFieldValue, values }) =>
				console.log('errors', errors) || console.log('values', values) || (
					<div>
						<Form noValidate onSubmit={handleSubmit}>
							<Section title={t('LayoutConfig:header-text')}>
								<div className="row">
									<div className="col-6 py-2">
										<CustomSelect
											inputId="layoutConfigLeft"
											isSearchable={false}
											isClearable={false}
											options={buttonLayoutsLeft}
											getOptionLabel={getOptionLabelLeft}
											className="form-select-sm sm-1 react-select__container"
											value={buttonLayoutsLeft.find((o) => o.value == values.buttonLayout)}
											errors={errors.buttonLayout}
											onChange={(o) => setValueLeft(o,values)}
										/>
									</div>
									<div className="col-6 py-2">
										<CustomSelect
											inputId="layoutConfigRight"
											isSearchable={false}
											isClearable={false}
											options={buttonLayoutsRight}
											getOptionLabel={getOptionLabelRight}
											className="form-select-sm sm-1 react-select__container"
											value={buttonLayoutsRight.find((o) => o.value == values.buttonLayoutRight)}
											errors={errors.buttonLayoutRight}
											onChange={(o) => setValueRight(o,values)}
										/>
									</div>
								</div>
								<div className="d-flex led-preview-container">
								<div className="button-layout-view col-auto">
										<GPController
											key="button-layout"
											className="led-preview"
											layouts={layoutSettings}
										>
										</GPController>
									</div>
                                    <div key="button-layout-properties" className="layout-properties col-3">
                                        <Accordion>
                                            <Accordion.Item>
                                                <Accordion.Header></Accordion.Header>
                                                <Accordion.Body></Accordion.Body>
                                            </Accordion.Item>
                                        </Accordion>
									</div>
								</div>
							</Section>
							<Button type="submit">{t('Common:button-save-label')}</Button>
							{saveMessage ? (
								<span className="alert">{saveMessage}</span>
							) : null}
							<FormContext/>
						</Form>
					</div>
				)
			}
		</Formik>
	);
}