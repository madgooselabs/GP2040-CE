import React, { useContext, useEffect, useState } from 'react';
import { AppContext } from '../Contexts/AppContext';
import { Button, Form, FormCheck, FormSelect, Table, Stack } from 'react-bootstrap';
import { Formik, useFormikContext, getIn } from 'formik';
import * as yup from 'yup';
import { useTranslation } from 'react-i18next';
import ContextualHelpOverlay from '../Components/ContextualHelpOverlay';

import Section from '../Components/Section';
import { GPController } from '../Components/ButtonLayouts';
import WebApi from '../Services/WebApi';
import boards from '../Data/Boards.json';
import {
	BUTTONS,
} from '../Data/Buttons';
import LEDColors from '../Data/LEDColors';

import './LayoutConfig.scss';

let layoutFieldsSchema = {};

const schema = yup.object().shape({
	...layoutFieldsSchema
});

let buttonLayouts = {ledLayout:{},displayLayouts:{buttonLayoutId:0,buttonLayout:{},buttonLayoutRightId:0,buttonLayoutRight:{}}};
let buttonLayoutDefinitions = {buttonLayout:{},buttonLayoutRight:{}};

const FormContext = () => {
	const { values, setValues } = useFormikContext();
	const { setLoading } = useContext(AppContext);

	useEffect(() => {
		async function fetchData() {
			await WebApi.getGamepadOptions(setLoading);
			buttonLayouts = await WebApi.getButtonLayouts(setLoading);
			buttonLayoutDefinitions = await WebApi.getButtonLayoutDefs(setLoading);
			console.dir(buttonLayouts);
			console.dir(buttonLayoutDefinitions);

			setValues({});
		}
		fetchData();
	}, [setValues]);

	useEffect(() => {
	}, [values, setValues]);

	return null;
};

export default function LayoutConfigPage() {
	const { buttonLabels, setButtonLabels, usedPins } = useContext(AppContext);
	const [saveMessage, setSaveMessage] = useState('');
	const [selectedElement, setSelectedElement] = useState(null);
	const { buttonLabelType } = buttonLabels;

	const onSuccess = async (values) => {
		const cleanValues = schema.cast(values);
		console.dir(cleanValues);

		setSaveMessage(
			success
				? t('Common:saved-success-message')
				: t('Common:saved-error-message'),
		);
	};

	const toggleSelectedElement = (e, el) => {
		e.stopPropagation();
		if (selectedElement === el) {
            el.setSelected(false);
            setSelectedElement(null);
		} else {
            if (selectedElement) selectedElement.setSelected(false);
			setSelectedElement(el);
            el.setSelected(true);
            console.dir(el.getDetails());
		}
	};

	const { t } = useTranslation('');

	return (
		<Formik onSubmit={onSuccess} validationSchema={schema} initialValues={{}}>
			{({ errors, handleSubmit, setFieldValue, values }) =>
				console.log('errors', errors) || (
					<div>
						<Form noValidate onSubmit={handleSubmit}>
							<Section title={t('LayoutConfig:header-text')}>
								<div className="row">
									<div className="col-6">
										<FormSelect
											key={`layoutConfigLeft`}
											id={`layoutConfigLeft`}
											name={`layoutConfigLeft`}
											className="form-select-sm sm-1"
											error={{}}
											value={buttonLayouts.displayLayouts.buttonLayoutId}
											onChange={(e) => {}}
										>
											{Object.keys(buttonLayoutDefinitions.buttonLayout).map((o, i) => (
												<option
													key={o}
													value={buttonLayoutDefinitions.buttonLayout[o]}
												>
													{t(`LayoutConfig:layouts.left.${o}`)}
												</option>
											))}
										</FormSelect>
									</div>
									<div className="col-6">
										<FormSelect
											key={`layoutConfigRight`}
											id={`layoutConfigRight`}
											name={`layoutConfigRight`}
											className="form-select-sm sm-1"
											error={{}}
											value={buttonLayouts.displayLayouts.buttonLayoutRightId}
											onChange={(e) => {}}
										>
											{Object.keys(buttonLayoutDefinitions.buttonLayoutRight).map((o, i) => (
												<option
													key={o}
													value={buttonLayoutDefinitions.buttonLayoutRight[o]}
												>
													{t(`LayoutConfig:layouts.right.${o}`)}
												</option>
											))}
										</FormSelect>
									</div>
								</div>
								<div className="d-flex led-preview-container align-items-start">
									<div className="button-layout-view col-auto">
										<GPController
											key="button-layout"
											className="led-preview"
											layouts={buttonLayouts}
											itemSelect={toggleSelectedElement}
										>
										</GPController>
									</div>
									<div className="button-layout-properties col-3">
										<div>
                                            <div className="button-layout-properties-grid-header row">
                                                <div className="grid-prop col-3">Property</div>
                                                <div className="grid-val col-9">Value</div>
                                            </div>
                                            {selectedElement ? 
                                                <div className="">
                                                     {Object.keys(selectedElement.getDetails()).map((o,i) => (
                                                    <div key={`properties-grid-value-${i}`} className="button-layout-properties-grid-value row">
                                                        <div className="grid-prop col-3">{o}</div>
                                                        <div className="grid-val col-9">{selectedElement.getDetails()[o]}</div>
                                                    </div>
                                                    ))}
                                                </div>
                                            : ''}
                                        </div>
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