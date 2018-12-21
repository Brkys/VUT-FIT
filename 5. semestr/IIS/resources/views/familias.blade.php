@extends('layouts.app')

@section('title', 'Familie')

@section('navpanel')
    @parent
@endsection

@section('content')
<div class="container" style="padding-top: 100px; width: 80% !important;"> 
    <table class="table table-hover table-dark text-centered" id="custom-table">
        <thead>
            <tr>
                <th scope="col" style="width: 33.3%;">Jméno</th>
                <th scope="col" style="width: 33.3%;">Don</th>
                <th scope="col" style="width: 33.3%;">Počet členů</th>
            </tr>
        </thead>
        <tbody>
        @if(!empty($results))
            @foreach($results as $result)
            <tr>
                <td>{{$result['JmenoFamilie']}}</td>
                <td>{{$result['JmenoDona']}}</td>
                <td>{{$result['members']}}</td>
            </tr>
            @endforeach
        @endif
        </tbody>
    </table>
</div>
@endsection