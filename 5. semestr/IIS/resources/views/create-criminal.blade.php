@extends('layouts.app')

@section('title', 'Vytvoření kriminální činnosti')

@section('navpanel')
	@parent
@endsection
@php if(empty(Session::get('criminalInput'))){
	$criminalInput = array(
		'activity_content' => '',
		'end_date' => ''
	);
  }
  else {
	$criminalInput = Session::get('criminalInput');
  }
@endphp

@section('content')
<div style="padding-top: 100px"></div>
<div class="container" style="width: 80% !important; background-color: white !important; border-radius: 4 !important;"> 
	<form action="createCriminal" method="post">
    	@csrf
	    <h4 style="padding-top: 20px;">Území</h4>
		<select class="form-control" id="sel1" name="land_id">
		@foreach($lands as $land)
    		<option value="{{$land->ID_Uzemi}}">{{$land->Adresa}}</option>
    	@endforeach
		</select>
		<h4 style="padding-top: 20px;">Popis činnosti</h4>
		<input class="form-control" id="myInput" type="text" value="{{$criminalInput['activity_content']}}" placeholder="Typ činnosti" name="activity_content"> 
		<h6 style="padding-top: 20px">Datum ukončení</h6>
	    <div class="input-group mb-3">
	        <div class="input-group-prepend">
	            <span class="input-group-text">YYYY-MM-DD</span>
	        </div>
			<input class="form-control" id="myInput" type="text" value="{{$criminalInput['end_date']}}" placeholder="Datum" name="end_date"> 
	    </div>
		@if($errors->any())
			<div class="alert alert-danger">
				<ul>
					@foreach ($errors->all() as $error)
						<li>{{ $error }}</li>
					@endforeach
				</ul>
			</div>  
        @endif
		<div style="padding-top: 20px"></div>
		<button type="submit" class="btn btn-primary btn-block" name="createCriminal">Potvrdit</button>
	</form>
<div style="padding-top: 20px"></div>
@endsection